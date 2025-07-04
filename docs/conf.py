from __future__ import annotations

from typing import Any

project = "phgasnets"
copyright = "2025, Max Planck Institute for Dynamics of Complex Technical Systems"
author = "Nayak, Ashwin S. and Bendokat, T."
version = release = "1.0.0"

extensions = [
    "myst_parser",
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.mathjax",
    "sphinx.ext.napoleon",
    "sphinx_autodoc_typehints",
    "sphinx_copybutton",
]

source_suffix = [".rst", ".md"]
exclude_patterns = [
    "_build",
    "**.ipynb_checkpoints",
    "Thumbs.db",
    ".DS_Store",
    ".env",
    ".venv",
]

html_theme = "sphinx_rtd_theme"

html_theme_options: dict[str, Any] = {
    "source_repository": "https://github.com/mpimd-csc/phgasnets",
    "source_branch": "main",
    "source_directory": "docs/",
}

autodoc_typehints = "description"
autodoc_typehints_format = "short"
autodoc_default_options = {
    'show-inheritance': True,
    'undoc-members': True,
    'member-order': 'bysource',
    'exclude-members': '',
    'no-value': True,  # This hides default values
}
myst_enable_extensions = [
    "colon_fence",
]

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
}

nitpick_ignore = [
    ("py:class", "_io.StringIO"),
    ("py:class", "_io.BytesIO"),
]

always_document_param_types = True
