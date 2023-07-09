"""
Priority Queue implementation as a C extension.
"""
import setuptools

setuptools.setup(
    packages=[
        "five_one_one_q",
    ],
    package_dir={
        "five_one_one_q": "python",
    },
    ext_modules=[
        setuptools.Extension(
            "five_one_one_q.c",
            ["c/bucketq.c"],
        ),
    ],
)
