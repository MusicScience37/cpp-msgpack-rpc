# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = "cpp-msgpack-rpc"
release = "unknown"  # inserted in command line

copyright = "2023, Kenta Kabashima"
author = "Kenta Kabashima"

# -- General configuration ---------------------------------------------------

extensions = [
    "sphinx.ext.todo",
]

templates_path = ["_templates"]

exclude_patterns = []

# -- Options for PlantUML ----------------------------------------------------

extensions += ["sphinxcontrib.plantuml"]

plantuml_output_format = "svg"
plantuml_syntax_error_image = True

# -- Options for MyST --------------------------------------------------------

extensions += ["myst_parser"]

myst_heading_anchors = 3

# -- Options for breathe -----------------------------------------------------

extensions += ["breathe"]

# this variable must be set from command line
breathe_projects = {"msgpack_rpc": ""}

breathe_default_project = "msgpack_rpc"

breathe_default_members = ("members",)

breathe_domain_by_extension = {
    "h": "cpp",
}

# -- Options for HTML output -------------------------------------------------

html_theme = "sphinx_orange_book_theme"
html_static_path = ["_static"]

html_title = project

html_theme_options = {
    "show_prev_next": False,
    "logo": {
        "text": html_title,
    },
    "pygment_light_style": "gruvbox-light",
    "pygment_dark_style": "native",
    "repository_url": "https://gitlab.com/MusicScience37Projects/utility-libraries/cpp-msgpack-rpc",
    "use_repository_button": True,
}
