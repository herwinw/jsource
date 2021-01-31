# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import divio_docs_theme

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'J Language: From C to C++'
copyright = '2021, Conor Hoekstra + contributors'
author = 'Conor Hoekstra + contributors'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [ "breathe" ]

breathe_projects = { 'j' : '/home/cph/jsource/build/docs/doxygen/xml'}

breathe_default_project = "j"

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
language = 'C++'

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#

html_favicon = '_static/j.jpg'
html_logo = '_static/j.jpg'
html_theme = 'divio_docs_theme'
html_theme_path = [divio_docs_theme.get_html_theme_path()]
html_theme_options = {
    # Options of read the docs theme
	'logo_only': False,
	'display_version': False,
	'prev_next_buttons_location': 'bottom',
	'style_external_links': True,
	# Toc options
	'collapse_navigation': True,
	'sticky_navigation': False,
	'navigation_depth': 3,
	'includehidden': True, # to show also hidden TOCs in the menu bar
	'titles_only': False,
    # 'canonical_url': "https://docs.nitrokey.com/",
    # Options of Divio (divio bases on read the docs theme)
    'analytics_id' : '',
    'vcs_pageview_mode' : '',
    'show_cloud_banner' : False, #False makes the divio box disappear
    'cloud_banner_url' : '',
    'cloud_banner_markup' : '',
    'segment_id' : ''
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']