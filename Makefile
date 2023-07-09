all:
	python setup.py sdist bdist_wheel

clean:
	rm -rf dist/ build/ *.egg-info .fuse_hidden*

install:
	python setup.py install

uninstall:
	pip uninstall -y five-one-one-q

reinstall:
	$(MAKE) clean
	$(MAKE) uninstall
	$(MAKE) install

test: reinstall
	python -m pytest
