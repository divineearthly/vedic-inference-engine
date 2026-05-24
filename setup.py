"""Setup for vedic-inference-engine — pip installable package."""

from setuptools import setup, find_packages

with open("README.md", "r") as f:
    long_description = f.read()

setup(
    name="vedic-inference-engine",
    version="1.0.0",
    author="Joydeep Das",
    author_email="joydeepdas.silchar@gmail.com",
    description="ॐ Unified Vedic algorithm imports — Nyaya, Guna, Rta-Dharma, Ahimsa-108, Antahkarana, Kosha-Net, Sphota Attention, Manas Gate, Para-Vak",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/divineearthly/vedic-inference-engine",
    packages=find_packages(),
    include_package_data=True,
    package_data={
        "vedic_inference_engine": ["../kernels/*.so", "../kernels/*.h"],
    },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: Python :: 3.13",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Mathematics",
    ],
    python_requires=">=3.10",
    install_requires=[],
    extras_require={
        "arm64": [],  # libsphota.so included for ARM64
    },
)
