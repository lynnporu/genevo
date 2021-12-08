import setuptools


with open("README.md", mode="r", encoding="utf-8") as file:
    long_description = file.read()

setuptools.setup(
    name="genevo",
    version="0.0.1",
    author="Pavlo Tymoshenko",
    author_email="p.tymoshen@gmail.com",
    description="Framework for evolutional neural networks",
    long_description=long_description,
    python_requires=">=3.9",
    long_description_content_type="text/markdown",
    url="https://github.com/lynnporu/genevo",
    classifiers=[
        "Programming Language :: Python :: 3.9",
        "Development Status :: 1 - Planning",
        "Intended Audience :: Information Technology",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Operating System :: POSIX :: Linux",
        "Topic :: Scientific/Engineering :: Artificial Intelligence"
    ],
    package_dir={"": "genevo"},
    packages=setuptools.find_packages(where="genevo")
)
