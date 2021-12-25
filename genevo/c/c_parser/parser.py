import re
import ctypes
import dataclasses

DECLARATION_START_TOKEN = "@declaration"
DECLARATION_UNIT_TOKEN = r"@(\w+)"

DECLARATION_TOKEN_NAME = "declaration"
RETURN_TOKEN_NAME = "return"
ARGUMENT_TOKEN_NAME = "argument"
MEMBER_TOKEN_NAME = "member"

COMMENT_SRE = r"(?:/?\*)?\s*"
TOKEN_SRE = r"(\w+\s*\**)"

DECLARATION_RE = re.compile(
    COMMENT_SRE + r"\s+".join([DECLARATION_START_TOKEN, TOKEN_SRE, TOKEN_SRE]))
DECLARATION_LINE_RE = re.compile(
    COMMENT_SRE + r"\s".join([DECLARATION_UNIT_TOKEN, TOKEN_SRE]))

END_COMMENT_RE = re.compile(r"\s*\*/")

BASE_C_TYPES = {
    "void": None,
    "uint8": ctypes.c_uint8,
    "uint16": ctypes.c_uint16,
    "uint32": ctypes.c_uint32,
    "uint64": ctypes.c_uint64,
    "char": ctypes.c_char
}


@dataclasses.dataclass
class CDeclaration:
    name: str

    @staticmethod
    def parse_lines(lines: str):
        declarations = list()
        for line in lines:
            name, value = DECLARATION_LINE_RE.match(line).groups()
            if name == DECLARATION_TOKEN_NAME:
                continue
            declarations.append((name, value))
        return declarations


@dataclasses.dataclass
class FunctionDeclaration(CDeclaration):
    return_type: str
    argument_types: list

    @classmethod
    def from_buffer(self, name: str, lines: str):
        return_type = None
        argument_types = list()

        for name, value in CDeclaration.parse_lines(lines):
            if name == RETURN_TOKEN_NAME:
                return_type = value
            elif name == ARGUMENT_TOKEN_NAME:
                argument_types.append(value)
            else:
                raise TypeError(
                    f"Unknown token `{name}` in function declaration.")

        return FunctionDeclaration(
            name=name,
            return_type=return_type,
            argument_types=argument_types)


@dataclasses.dataclass
class StructureDeclaration(CDeclaration):
    members: list

    @classmethod
    def from_buffer(self, name: str, lines: str):
        members = list()

        for name, value in CDeclaration.parse_lines(lines):
            if name == MEMBER_TOKEN_NAME:
                members.append(value)
            else:
                raise TypeError(
                    f"Unknown token `{name}` in structure declaration.")

        return StructureDeclaration(name=name, members=members)


@dataclasses.dataclass
class VariableDeclaration(CDeclaration):
    type_name: str


class FileParser:

    def __init__(self):
        self.declarations = dict()

    def parse_file(self, address: str):

        with open(address, mode="r") as file:
            lines = "".join(file.readlines()).split("\n")

        buffer = list()
        is_declaration_section = False

        for line in lines:

            if END_COMMENT_RE.match(line):
                if not is_declaration_section:
                    continue
                is_declaration_section = False
                self.parse_declaration(buffer)

            if DECLARATION_START_TOKEN in line:
                is_declaration_section = True

            if is_declaration_section:
                buffer.append(line)

    def parse_declaration(self, buffer: list):

        dec_type, name = DECLARATION_RE.match(buffer[0]).groups()

        declarators = {
            "function": FunctionDeclaration,
            "struct": StructureDeclaration,
            "variable": VariableDeclaration
        }

        if dec_type not in declarators:
            raise TypeError(
                f"Theres unknown `{dec_type}` declaration in the file.")

        self.declarations[name] = \
            declarators[dec_type].from_buffer(name, buffer)
