# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import sys, subprocess, os



project = 'esp-idf-ds3221' 
copyright = '2024, Jason M. Schwefel'
author = 'Jason M. Schwefel'
release = '0.9.10'

read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
if read_the_docs_build:
     subprocess.call('cd ../; doxygen', shell=True)

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

autodoc_member_order = 'bysource'

extensions = []

templates_path = ['_templates']
exclude_patterns = []

sys.path.append("/usr/bin/")
breathe_default_project = "esp-idf-ds3231"

extensions = ['sphinx_copybutton', 'sphinx.ext.todo', 'breathe', 'sphinx_rtd_theme']

project_homepage = "https://github.com/jschwefel/esp-idf-ds3231"

breathe_projects = {"esp-idf-ds3231": "../xml" }

breathe_debug_trace_directives = True
breathe_show_include = False
breathe_order_parameters_first = False
breathe_show_define_initializer = True
breathe_default_members = ('members', 'undoc-members')
breathe_domain_by_extension = {
    "h" : "c",
}

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_css_files = [
    'custom-signame.css',
]


