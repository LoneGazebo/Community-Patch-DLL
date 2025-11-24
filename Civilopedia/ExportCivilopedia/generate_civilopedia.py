#!/usr/bin/env python3
"""
Generate HTML Civilopedia from exported JSON data.

Usage:
    - Make sure logging is enabled.
    - Copy "ExportCivilopedia" to your mods folder and start a game with VP and the mod "Export Civilopedia" loaded
    - When clicking on "Begin your journey", the game data are written to the lua log.
    - in [git-repo]/Civilopedia/ExportCivilopedia/ run "python extract_from_log.py", which will create a json file with the game data
    - run "python generate_civilopedia.py"

"""

import json
import os
import sys
import shutil
import struct
import numpy as np
from pathlib import Path
from jinja2 import select_autoescape
from PIL import Image as PILImage
from wand.image import Image as WandImage



# Load localization strings
def load_localization(lang='en_US'):
    """Load localization strings from JSON file"""
    loc_path = Path(__file__).parent / 'Text' / lang / 'UI.json'
    with open(loc_path, 'r', encoding='utf-8') as f:
        return json.load(f)

# Load English localization
LOC = load_localization('en_US')

# JSON Input file name
JSON_INPUT_FILE_NAME = "civilopedia_export.json"

# Output folder
OUTPUT_FOLDER = "../generated_files"

# Version information
CIVILOPEDIA_VERSION = LOC['CIVILOPEDIA_VERSION']

# Folder for icons from the BNW atlases
BNW_ICONS_FOLDER = "../bnw_icons"

# Folder for generated icons (icons from VP)
GENERATED_ICONS_FOLDER = "../generated_icons"

# BNW atlases don't change so they usually don't need to be regenerated.
# If you do need to regenerate them, put the BNW dds files into BNW_ATLASES_FOLDER.
# To extract the dds files from the game use 'Nexus' from the SDK
# (run the script 'Nexus > Script Browser > Extract PAK files' after setting the asset
# base path to the path of the Civ 5 folder).
BNW_ATLASES_FOLDER = "../../Atlases"


# Define paths to search for atlas files
ATLAS_BASE_PATHS = [
    "../../(2) Vox Populi/Assets/Atlases"
]

# HTML template for the main index page
INDEX_TEMPLATE = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{ civilopediaTitle }}</title>
    <link rel="stylesheet" href="css/style.css">
</head>
<body>
    <div class="container">
        <header>
            <h1>{{ civilopediaTitle }}</h1>
            <div class="search-container">
                <input type="text" id="globalSearch" placeholder="{{ searchPlaceholder }}" autocomplete="off">
                <div id="searchResults" class="search-results"></div>
            </div>
        </header>

        <nav class="main-nav">
            <a href="index.html" class="active">{{ labels.get('civilopediaCategoryHome') }}</a>
            <a href="concepts.html">{{ labels.get('civilopediaCategoryGameConcepts') }}</a>
            <a href="civilizations.html">{{ labels.get('civilopediaCategoryCivilizations') }}</a>
            <a href="units.html">{{ labels.get('civilopediaCategoryUnits') }}</a>
            <a href="buildings.html">{{ labels.get('civilopediaCategoryBuildings') }}</a>
            <a href="wonders.html">{{ labels.get('civilopediaCategoryWonders') }}</a>
            <a href="technologies.html">{{ labels.get('civilopediaCategoryTechnologies') }}</a>
            <a href="policies.html">{{ labels.get('civilopediaCategoryPolicies') }}</a>
            <a href="beliefs.html">{{ labels.get('civilopediaCategoryReligion') }}</a>
            <a href="promotions.html">{{ labels.get('civilopediaCategoryPromotions') }}</a>
            <a href="resources.html">{{ labels.get('civilopediaCategoryResources') }}</a>
            <a href="improvements.html">{{ labels.get('civilopediaCategoryImprovements') }}</a>
            <a href="terrain.html">{{ labels.get('civilopediaCategoryTerrains') }}</a>
            <a href="greatPeople.html">{{ labels.get('civilopediaCategorySpecialists') }}</a>
            <a href="cityStates.html">{{ labels.get('civilopediaCategoryCityStates') }}</a>
            <a href="worldCongress.html">{{ labels.get('civilopediaCategoryWorldCongress') }}</a>
            <a href="corporations.html">{{ labels.get('civilopediaCategoryCorporations') }}</a>
        </nav>

        <main>
            <section class="welcome">
                <h2>{{ welcomeMessage }}</h2>
                <p>{{ currentVersion }} {{ version }}</p>
                {% if quoteText %} 
                <div class="text-box" style="margin-top: 1.5rem;">
                    {{ quoteText | civ5format | safe }}
                </div>
                {% endif %}
                {% if helpText %}
                <div class="text-box" style="margin-top: 1.5rem;">
                    {{ helpText | civ5format | safe }}
                </div>
                {% endif %}
            </section>
        </main>

        <footer>
            <p>{{ civilopediaTitle }} - Version {{ version }}</p>
        </footer>
    </div>
    <script src="searchIndex.js"></script>
    <script>
        // Search functionality for index page
        const searchInput = document.getElementById('globalSearch');
        const searchResults = document.getElementById('searchResults');
        let selectedIndex = -1;

        if (searchInput && window.globalSearchIndex) {
            searchInput.addEventListener('input', function(e) {
                const query = e.target.value.trim().toLowerCase();
                selectedIndex = -1; // Reset selection on new input

                if (query.length < 2) {
                    searchResults.style.display = 'none';
                    searchResults.innerHTML = '';
                    return;
                }

                // Search for matches in article titles across all categories
                const matches = window.globalSearchIndex.filter(article =>
                    article.name.toLowerCase().includes(query)
                ).slice(0, 10); // Limit to 10 results

                if (matches.length > 0) {
                    // Clear previous results
                    searchResults.innerHTML = '';

                    // Create and append each result element
                    matches.forEach(match => {
                        const resultDiv = document.createElement('div');
                        resultDiv.className = 'search-result-item';
                        resultDiv.setAttribute('data-page', match.page);
                        resultDiv.setAttribute('data-type', match.type);

                        const strong = document.createElement('strong');
                        strong.innerHTML = match.nameFormatted; // Use formatted name with HTML/colors
                        resultDiv.appendChild(strong);

                        // Add click handler
                        resultDiv.addEventListener('click', function() {
                            navigateToArticle(match.page, match.type);
                        });

                        searchResults.appendChild(resultDiv);
                    });
                    searchResults.style.display = 'block';
                } else {
                    searchResults.innerHTML = '<div class="search-result-item no-results">{{ noResultsFound }}</div>';
                    searchResults.style.display = 'block';
                }
            });

            // Keyboard navigation
            searchInput.addEventListener('keydown', function(e) {
                const items = searchResults.querySelectorAll('.search-result-item:not(.no-results)');

                if (items.length === 0) return;

                if (e.key === 'ArrowDown') {
                    e.preventDefault();
                    selectedIndex = Math.min(selectedIndex + 1, items.length - 1);
                    updateSelection(items);
                } else if (e.key === 'ArrowUp') {
                    e.preventDefault();
                    selectedIndex = Math.max(selectedIndex - 1, -1);
                    updateSelection(items);
                } else if (e.key === 'Enter') {
                    e.preventDefault();
                    if (selectedIndex >= 0 && selectedIndex < items.length) {
                        const selectedItem = items[selectedIndex];
                        const page = selectedItem.getAttribute('data-page');
                        const type = selectedItem.getAttribute('data-type');
                        navigateToArticle(page, type);
                    }
                } else if (e.key === 'Escape') {
                    searchResults.style.display = 'none';
                    selectedIndex = -1;
                }
            });

            function updateSelection(items) {
                items.forEach((item, index) => {
                    if (index === selectedIndex) {
                        item.classList.add('selected');
                        item.scrollIntoView({ block: 'nearest' });
                    } else {
                        item.classList.remove('selected');
                    }
                });
            }

            // Close search results when clicking outside
            document.addEventListener('click', function(e) {
                if (!e.target.closest('.search-container')) {
                    searchResults.style.display = 'none';
                    selectedIndex = -1;
                }
            });

            // Prevent closing when clicking inside search container
            searchInput.addEventListener('click', function(e) {
                e.stopPropagation();
            });
        }

        // Global keyboard shortcut: Press 'S' to focus search
        document.addEventListener('keydown', function(e) {
            // Check if 'S' key is pressed and search input is not already focused
            if (e.key === 's' || e.key === 'S') {
                // Don't trigger if user is typing in an input field
                if (document.activeElement.tagName === 'INPUT' ||
                    document.activeElement.tagName === 'TEXTAREA' ||
                    document.activeElement.isContentEditable) {
                    return;
                }
                e.preventDefault();
                searchInput.focus();
            }
        });

        function navigateToArticle(page, itemType) {
            window.location.href = page + '.html#item-' + itemType;
        }

        function escapeHtml(text) {
            const div = document.createElement('div');
            div.textContent = text;
            return div.innerHTML;
        }
    </script>
</body>
</html>
"""

# Template for civilization sidebar page
# Removed - now using unified SIDEBAR_TEMPLATE


# CSS styles
CSS_TEMPLATE = """* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background: #283a4c;
    color: #333;
    line-height: 1.6;
    min-height: 100vh;
}

.container {
    max-width: 1200px;
    margin: 0 auto;
    background: #283a4c;
    min-height: 100vh;
    box-shadow: 0 0 40px rgba(0, 0, 0, 0.3);
}

header {
    background: #283a4c;
    color: white;
    padding: 2rem;
    text-align: center;
    box-shadow: 0 2px 10px rgba(0, 0, 0, 0.2);
}

header h1 {
    font-size: 2.5rem;
    margin-bottom: 0.5rem;
    text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
}

.subtitle {
    font-size: 1.1rem;
    opacity: 0.9;
}

.main-nav {
    display: flex;
    flex-wrap: wrap;
    background: #34495e;
    padding: 0;
    box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
}

.main-nav a {
    color: white;
    text-decoration: none;
    padding: 1rem 1.5rem;
    transition: background 0.3s;
    border-bottom: 3px solid transparent;
}

.main-nav a:hover {
    background: #3e5568;
}

.main-nav a.active {
    background: #3e5568;
}

main {
    padding: 2rem;
    min-height: calc(100vh - 300px);
}

.welcome {
    background: #34495e;
    color: white;
    padding: 2rem;
    border-radius: 10px;
    margin-bottom: 2rem;
    box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
}

.welcome h2 {
    margin-bottom: 1rem;
    font-size: 2rem;
}


.search-box {
    margin-bottom: 2rem;
}

#searchInput {
    width: 100%;
    padding: 1rem;
    font-size: 1rem;
    border: 2px solid #ddd;
    border-radius: 5px;
    transition: border-color 0.3s;
}

#searchInput:focus {
    outline: none;
    border-color: #3498db;
}

/* Concepts sidebar layout */
.pediapage-layout {
    display: flex;
    gap: 0;
    min-height: 600px;
    padding: 0;
}

.pediapage-sidebar {
    width: 300px;
    background: #283a4c;
    border-right: 2px solid #ddd;
    overflow-y: auto;
    max-height: calc(100vh - 200px);
    position: sticky;
    top: 0;
}

.sidebar-header {
    background: #34495e;
    color: white;
    padding: 0.35rem 1rem;
    font-weight: bold;
    cursor: pointer;
    user-select: none;
    border-bottom: 1px solid #eee;
    border-top: 1px solid #eee;
}

.sidebar-section {
    border-bottom: 1px solid #ddd;
}

.sidebar-section-header {
    background: #34495e;
    color: white;
    padding: 0.35rem 1rem;
    font-weight: bold;
    cursor: pointer;
    user-select: none;
    border-bottom: 1px solid #eee;
    transition: background 0.2s;
}

.sidebar-section-header:hover {
    background: #3e5568;
}

.sidebar-section-header .section-icon {
    display: inline-block;
    width: 1em;
    transition: transform 0.2s;
}

.sidebar-section.collapsed .sidebar-section-header {
    border-bottom: none;
}

.sidebar-list {
    list-style: none;
    padding: 0;
    margin: 0;
}

.sidebar-list li {
    border-bottom: 1px solid #eee;
}

.sidebar-list li:last-child {
    border-bottom: none;
}

.sidebar-list a {
    display: block;
    padding: 0.2rem 0.75rem;
    color: white;
    text-decoration: none;
    transition: background 0.2s;
    font-size: 0.9rem;
}

.sidebar-list a:hover {
    background: #3e5568;
    color: white;
}

.sidebar-list a.active {
    background: #3e5568;
    color: white;
}

.pediapage-content {
    flex: 1;
    padding: 2rem;
    overflow-y: auto;
    max-height: calc(100vh - 200px);
}

.placeholder {
    color: white;
}

.placeholder h2 {
    color: white;
    font-size: 2rem;
    margin-bottom: 1.5rem;
    padding-bottom: 0.5rem;
    border-bottom: 3px solid #3498db;
}

.article-detail {
    animation: fadeIn 0.3s;
}

@keyframes fadeIn {
    from { opacity: 0; }
    to { opacity: 1; }
}

.article-detail h2 {
    color: white;
    font-size: 2rem;
    margin-bottom: 1.5rem;
    padding-bottom: 0.5rem;
    border-bottom: 3px solid #3498db;
}

.summary {
    margin-bottom: 1.5rem;
}

.summary h3 {
    color: white;
    font-size: 1.3rem;
    margin-bottom: 1rem;
}

.text-box {
    background: #283a4c;
    border-left: 4px solid #3498db;
    padding: 1.5rem;
    border-radius: 5px;
    line-height: 1.8;
    text-align: justify;
}

.text-section {
    background: white;
}

/* Links in text-box areas (resource details, etc.) */
.text-box a {
    color: #3498db;
    text-decoration: none;
    transition: color 0.3s;
}

.text-box a:hover {
    color: #2980b9;
    text-decoration: underline;
}

.text-box a:visited {
    color: #9b59b6;
}

footer {
    background: #34495e;
    color: white;
    text-align: center;
    padding: 0.75rem;
    margin-top: 2rem;
}

/* Civ5 formatting colors */

.green {
    color: #4caf50;
}

.positive {
    color: #4caf50;
    font-weight: bold;
}


.negative {
    color: #f44336;
    font-weight: bold;
}

.warning {
    color: #ff9800;
    font-weight: bold;
}

.cyan {
    color: #00bcd4;
}

.yellow {
    color: #ffeb3b;
}

.magenta {
    color: #e91e63;
}

.white {
    color: #9e9e9e;
}

.player-dark-green {
    color: #2e7d32;
    font-weight: bold;
}

.yield-food {
    color: #8bc34a;
    font-weight: bold;
}

.yield-production {
    color: #ff9800;
    font-weight: bold;
}

.yield-gold {
    color: #ffc107;
    font-weight: bold;
}

.yield-science {
    color: #2196f3;
    font-weight: bold;
}

.yield-culture {
    color: #9c27b0;
    font-weight: bold;
}

.yield-faith {
    color: #cddc39;
    font-weight: bold;
}

@media (max-width: 768px) {
    .main-nav {
        flex-direction: column;
    }

    .items-grid {
        grid-template-columns: 1fr;
    }

    header h1 {
        font-size: 1.8rem;
    }
}

/* Search bar styles */
.search-container {
    position: relative;
    margin-top: 1rem;
    max-width: 500px;
    margin-left: auto;
    margin-right: auto;
}

#globalSearch {
    width: 100%;
    padding: 0.75rem 1rem;
    font-size: 1rem;
    border: 2px solid #34495e;
    border-radius: 5px;
    background: white;
    color: #333;
    transition: border-color 0.3s;
}

#globalSearch:focus {
    outline: none;
    border-color: #5dade2;
}

#globalSearch::placeholder {
    color: #999;
}

.search-results {
    position: absolute;
    top: 100%;
    left: 0;
    right: 0;
    background: white;
    border: 2px solid #34495e;
    border-top: none;
    border-radius: 0 0 5px 5px;
    max-height: 400px;
    overflow-y: auto;
    display: none;
    z-index: 1000;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
}

.search-result-item {
    padding: 0.75rem 1rem;
    cursor: pointer;
    border-bottom: 1px solid #eee;
    transition: background 0.2s;
}

.search-result-item:last-child {
    border-bottom: none;
}

.search-result-item:hover {
    background: #f0f0f0;
}

.search-result-item.selected {
    background: #3498db;
    color: white;
}

.search-result-item.selected strong {
    color: white;
}

.search-result-item.no-results {
    color: #999;
    cursor: default;
}

.search-result-item.no-results:hover {
    background: white;
}

.search-result-item strong {
    color: #333;
    font-weight: 600;
}
"""


# Generic sidebar template (for all categories with in-game layout)
SIDEBAR_TEMPLATE = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{ title }} - {{ civilopediaTitle }}</title>
    <link rel="stylesheet" href="css/style.css">
    <script src="searchIndex.js"></script>
</head>
<body>
    <div class="container">
        <header>
            <h1>{{ title }}</h1>
            <div class="search-container">
                <input type="text" id="globalSearch" placeholder="{{ searchPlaceholder }}" autocomplete="off">
                <div id="searchResults" class="search-results"></div>
            </div>
        </header>

        <nav class="main-nav">
            <a href="index.html">{{ labels.get('civilopediaCategoryHome') }}</a>
            <a href="concepts.html"{% if active_page == "concepts" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryGameConcepts') }}</a>
            <a href="civilizations.html"{% if active_page == "civilizations" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryCivilizations') }}</a>
            <a href="units.html"{% if active_page == "units" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryUnits') }}</a>
            <a href="buildings.html"{% if active_page == "buildings" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryBuildings') }}</a>
            <a href="wonders.html"{% if active_page == "wonders" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryWonders') }}</a>
            <a href="technologies.html"{% if active_page == "technologies" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryTechnologies') }}</a>
            <a href="policies.html"{% if active_page == "policies" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryPolicies') }}</a>
            <a href="beliefs.html"{% if active_page == "beliefs" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryReligion') }}</a>
            <a href="promotions.html"{% if active_page == "promotions" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryPromotions') }}</a>
            <a href="resources.html"{% if active_page == "resources" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryResources') }}</a>
            <a href="improvements.html"{% if active_page == "improvements" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryImprovements') }}</a>
            <a href="terrain.html"{% if active_page == "terrain" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryTerrains') }}</a>
            <a href="greatPeople.html"{% if active_page == "greatPeople" %} class="active"{% endif %}>{{ labels.get('civilopediaCategorySpecialists') }}</a>
            <a href="cityStates.html"{% if active_page == "cityStates" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryCityStates') }}</a>
            <a href="worldCongress.html"{% if active_page == "resolutions" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryWorldCongress') }}</a>
            <a href="corporations.html"{% if active_page == "corporations" %} class="active"{% endif %}>{{ labels.get('civilopediaCategoryCorporations') }}</a>
        </nav>

        <main class="pediapage-layout">
            <aside class="pediapage-sidebar">
                <div class="sidebar-header" onclick="showHomePage(); return false;" style="cursor: pointer;">{{ pageLabel | civ5format | safe }}</div>
                    {% for group_name, group_items in grouped_items %}
                    {% if group_name %}
                    <div class="sidebar-section collapsed">
                        <div class="sidebar-section-header" onclick="toggleSection(this)">
                            <span class="section-icon">▶</span> {{ group_name | civ5format | safe }}
                        </div>
                        <ul class="sidebar-list" style="display: none;">
                            {% for item in group_items %}
                            <li><a href="#" onclick="showItem('{{ item.Type }}'); return false;">{{ (item.Name if item.Name else item.Description) | civ5format | safe }}</a></li>
                            {% endfor %}
                        </ul>
                    </div>
                    {% else %}
                    <ul class="sidebar-list">
                        {% for item in group_items %}
                        <li><a href="#" onclick="showItem('{{ item.Type }}'); return false;">{{ (item.Name if item.Name else item.Description) | civ5format | safe }}</a></li>
                        {% endfor %}
                    </ul>
                    {% endif %}
                    {% endfor %}
            </aside>

            <div class="pediapage-content" id="itemContent">
                <div class="placeholder">
                    <section class="welcome">
                        <h2>{{ title }}</h2>
                        {% if quoteText %}
                        <div class="summary">
                            <div class="text-box">
                                {{ quoteText | civ5format | safe }}
                            </div>
                        </div>
                        {% endif %}
                        {% if helpText %}
                        <div class="summary">
                            <div class="text-box">
                                {{ helpText | civ5format | safe }}
                            </div>
                        </div>
                        {% endif %}
                    </section>
                </div>
                {% for group_name, group_items in grouped_items %}
                    {% for item in group_items %}
                    <div class="article-detail" id="item-{{ item.Type }}" style="display: none;">
                        <section class="welcome">
                            <div style="display: flex; align-items: center; gap: 1rem; margin-bottom: 1rem;">
                                {% if item.IconPath %}
                                <img src="{{ item.IconPath }}" alt="{{ (item.Name if item.Name else item.Description) }}" style="width: 64px; height: 64px; image-rendering: pixelated;" />
                                {% endif %}
                                <h2 style="margin: 0;">{{ (item.Name if item.Name else item.Description) | civ5format | safe }}</h2>
                            </div>
                            {% if item.Help %}
                            <div class="summary">
                                <h3>{{ labels.get('gameInfoLabel') }}</h3>
                                <div class="text-box">
                                    {{ item.Help | civ5format | safe }}
                                </div>
                            </div>
                            {% endif %}
                            {% if active_page == "resources" %}
                                <div class="summary">
                                    <h3>{{ labels.get('baseYieldsHeader') }}</h3>
                                    <div class="text-box">
                                        {% if item.BaseYields and item.BaseYields|length > 0 %}
                                            {% for yield in item.BaseYields %}
                                                {% if yield.Yield > 0 %}+{% endif %}{{ yield.Yield }} {{ yield.YieldName | civ5format | safe}}{% if not loop.last %}, {% endif %}
                                            {% endfor %}
                                        {% else %}
                                            {{ labels.get('noYieldText') }}
                                        {% endif %}
                                    </div>
                                </div>
                                {% if item.Improvements and item.Improvements|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('improvedByHeader') }}</h3>
                                    <div class="text-box">
                                        {% for improvement in item.Improvements %}
                                            <div style="margin-bottom: 8px;">
                                                <strong><a href="#" onclick="showImprovement('{{ improvement.Type }}'); return false;">{{ improvement.Name }}</a>:</strong>
                                                {% if improvement.Yields and improvement.Yields|length > 0 %}
                                                    {% for yield in improvement.Yields %}
                                                        {% if yield.Yield > 0 %}+{% endif %}{{ yield.Yield }} {{ yield.YieldName | civ5format | safe}}{% if not loop.last %}, {% endif %}
                                                    {% endfor %}
                                                {% endif %}
                                            </div>
                                        {% endfor %}
                                    </div>
                                </div>
                                {% endif %}
                                {% if (item.Terrains and item.Terrains|length > 0) or (item.Features and item.Features|length > 0) %}
                                <div class="summary">
                                    <h3>{{ labels.get('foundOnHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for terrain in item.Terrains %}
                                            <li><a href="#" onclick="showTerrain('{{ terrain.Type }}'); return false;">{{ terrain.Name }}</a></li>
                                            {% endfor %}
                                            {% for feature in item.Features %}
                                            <li><a href="#" onclick="showTerrain('{{ feature.Type }}'); return false;">{{ feature.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                            {% endif %}
                            {% if item.TechRevealName %}
                                <div class="summary">
                                    <h3>{{ labels.get('revealedByHeader') }}</h3>
                                    <div class="text-box">
                                        <a href="#" onclick="showTech('{{ item.TechReveal }}'); return false;">{{ item.TechRevealName }}</a>
                                    </div>
                                </div>
                                {% endif %}
                            {% if active_page == "improvements" %}
                                {% if item.PrereqTechName %}
                                <div class="summary">
                                    <h3>{{ labels.get('requiredTechnologyHeader') }}</h3>
                                    <div class="text-box">
                                        <a href="#" onclick="showTechnology('{{ item.PrereqTech }}'); return false;">{{ item.PrereqTechName }}</a>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.BaseYields and item.BaseYields|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('baseYieldsHeader') }}</h3>
                                    <div class="text-box">
                                        {% for yield in item.BaseYields %}
                                            {% if yield.Yield > 0 %}+{% endif %}{{ yield.Yield }} {{ yield.YieldName | civ5format | safe}}{% if not loop.last %}, {% endif %}
                                        {% endfor %}
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.AdjacencyBonuses and item.AdjacencyBonuses|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('adjacencyBonusesHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for group in item.AdjacencyBonuses|group_adjacency_bonuses %}
                                            <li>
                                                <strong>{{ group.Description }}:</strong>
                                                {% for yield in group.Yields %}
                                                    {% if yield.Yield > 0 %}+{% endif %}{{ yield.Yield|format_yield }} {{ yield.YieldName | civ5format | safe }}{% if not loop.last %}, {% endif %}
                                                {% endfor %}
                                            </li>
                                            {% endfor %}
                                        </ul>
                                        {% set has_fractional = namespace(value=false) %}
                                        {% for bonus in item.AdjacencyBonuses %}
                                            {% if bonus.Yield != (bonus.Yield|int) %}
                                                {% set has_fractional.value = true %}
                                            {% endif %}
                                        {% endfor %}
                                        {% if has_fractional.value %}
                                        <p style="margin-top: 1rem;">{{ labels.get('adjacentYieldsExplanation') }}</p>
                                        {% endif %}
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.RouteBonuses and item.RouteBonuses|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('cityConnectionBonusesHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for group in item.RouteBonuses|group_adjacency_bonuses %}
                                            <li>
                                                <strong>{{ group.Description }}:</strong>
                                                {% for yield in group.Yields %}
                                                    {% if yield.Yield > 0 %}+{% endif %}{{ yield.Yield|format_yield }} {{ yield.YieldName | civ5format | safe }}{% if not loop.last %}, {% endif %}
                                                {% endfor %}
                                            </li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                                {% if (item.Terrains and item.Terrains|length > 0) or (item.Features and item.Features|length > 0) %}
                                <div class="summary">
                                    <h3>{{ labels.get('canBeBuiltOnHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for terrain in item.Terrains %}
                                            <li><a href="#" onclick="showTerrain('{{ terrain.Type }}'); return false;">{{ terrain.Name }}</a></li>
                                            {% endfor %}
                                            {% for feature in item.Features %}
                                            <li><a href="#" onclick="showTerrain('{{ feature.Type }}'); return false;">{{ feature.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.Resources and item.Resources|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('improvesResourcesHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for resource in item.Resources %}
                                            <li><a href="#" onclick="showResource('{{ resource.Type }}'); return false;">{{ resource.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                            {% endif %}
                            {% if active_page == "terrain" %}
                                <div class="summary">
                                    <h3>{{ labels.get('baseYieldsHeader') }}</h3>
                                    <div class="text-box">
                                        {% if item.BaseYields and item.BaseYields|length > 0 %}
                                            {% for yield in item.BaseYields %}
                                                {{ yield.Yield }} {{ yield.YieldName | civ5format | safe}}{% if not loop.last %}, {% endif %}
                                            {% endfor %}
                                        {% else %}
                                            {{ labels.get('noYieldText') }}
                                        {% endif %}
                                    </div>
                                </div>
                                {% if item.EraYields and item.EraYields|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('yieldIncreasePerEraHeader') }}</h3>
                                    <div class="text-box">
                                        {% for yield in item.EraYields %}
                                            {% if yield.Yield > 0 %}+{% endif %}{{ yield.Yield }} {{ yield.YieldName | civ5format | safe}}{% if not loop.last %}, {% endif %}
                                        {% endfor %}
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.MovementCost %}
                                <div class="summary">
                                    <h3>{{ labels.get('movementCostHeader') }}</h3>
                                    <div class="text-box">
                                        {% if item.IsImpassable %}
                                            {{ labels.get('impassableText') }}
                                        {% else %}
                                            {{ item.MovementCost }} 》
                                        {% endif %}
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.DefenseModifier is defined %}
                                <div class="summary">
                                    <h3>{{ labels.get('combatModifierHeader') }}</h3>
                                    <div class="text-box">
                                        {% if item.DefenseModifier > 0 %}+{% endif %}{{ item.DefenseModifier }}% 
                                    </div>
                                </div>
                                {% endif %}
                                {% if (item.Terrains and item.Terrains|length > 0) or (item.Features and item.Features|length > 0) %}
                                <div class="summary">
                                    <h3>{{ labels.get('canAppearOnHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for terrain in item.Terrains %}
                                            <li><a href="#" onclick="showTerrain('{{ terrain.Type }}'); return false;">{{ terrain.Name }}</a></li>
                                            {% endfor %}
                                            {% for feature in item.Features %}
                                            <li><a href="#" onclick="showTerrain('{{ feature.Type }}'); return false;">{{ feature.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.Resources and item.Resources|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('resourcesFoundHereHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for resource in item.Resources %}
                                            <li><a href="#" onclick="showResource('{{ resource.Type }}'); return false;">{{ resource.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                            {% endif %}
                            {% if title == "Great People" %}
                                {% if item.YieldChanges and item.YieldChanges|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('baseYieldsHeader') }}</h3>
                                    <div class="text-box">
                                        {% for yield in item.YieldChanges %}
                                            {% if yield.Yield > 0 %}+{% endif %}{{ yield.Yield }} {{ yield.YieldName | civ5format | safe}}{% if not loop.last %}, {% endif %}
                                        {% endfor %}
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.GreatPeopleUnit %}
                                <div class="summary">
                                    <h3>{{ labels.get('generatesGreatPersonHeader') }}</h3>
                                    <div class="text-box">
                                        <a href="#" onclick="showUnit('{{ item.GreatPeopleUnit.Type }}'); return false;">{{ item.GreatPeopleUnit.Name }}</a>
                                    </div>
                                </div>
                                {% endif %}
                            {% endif %}
                            {% if title == "Corporations" %}
                                {% if item.ResourceBonusHelp %}
                                <div class="summary">
                                    <h3>{{ labels.get('resourceBonusesHeader') }}</h3>
                                    <div class="text-box">
                                        {{ item.ResourceBonusHelp | civ5format | safe }}
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.OfficeBonusHelp %}
                                <div class="summary">
                                    <h3>{{ labels.get('officeBonusesHeader') }}</h3>
                                    <div class="text-box">
                                        {{ item.OfficeBonusHelp | civ5format | safe }}
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.TradeRouteBonusHelp %}
                                <div class="summary">
                                    <h3>{{ labels.get('tradeRouteBonusesHeader') }}</h3>
                                    <div class="text-box">
                                        {{ item.TradeRouteBonusHelp | civ5format | safe }}
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.MonopolyResources and item.MonopolyResources|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('requiredMonopolyResourcesHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for resource in item.MonopolyResources %}
                                            <li><a href="#" onclick="showResource('{{ resource.Type }}'); return false;">{{ resource.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.HeadquartersBuilding %}
                                <div class="summary">
                                    <h3>{{ labels.get('headquartersHeader') }}</h3>
                                    <div class="text-box">
                                        <a href="#" onclick="showWonder('{{ item.HeadquartersBuilding.Type }}'); return false;">{{ item.HeadquartersBuilding.Name }}</a>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.OfficeBuilding %}
                                <div class="summary">
                                    <h3>{{ labels.get('officeHeader') }}</h3>
                                    <div class="text-box">
                                        <a href="#" onclick="showBuilding('{{ item.OfficeBuilding.Type }}'); return false;">{{ item.OfficeBuilding.Name }}</a>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.FranchiseBuilding %}
                                <div class="summary">
                                    <h3>{{ labels.get('franchiseHeader') }}</h3>
                                    <div class="text-box">
                                        <a href="#" onclick="showBuilding('{{ item.FranchiseBuilding.Type }}'); return false;">{{ item.FranchiseBuilding.Name }}</a>
                                    </div>
                                </div>
                                {% endif %}
                            {% endif %}
                            {% if item.Summary %}
                            <div class="summary">
                                <h3>{{ labels.get('summaryHeader') }}</h3>
                                <div class="text-box">
                                    {{ item.Summary | civ5format | safe }}
                                </div>
                            </div>
                            {% endif %}
                            {% if item.RequiredBuildings and item.RequiredBuildings|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('requiredBuildingsHeader') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for building in item.RequiredBuildings %}
                                        <li><a href="#" onclick="showBuilding('{{ building.Type }}'); return false;">{{ building.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.LeadsToBuildings and item.LeadsToBuildings|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('leadsToBldgLabel') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for building in item.LeadsToBuildings %}
                                        <li><a href="#" onclick="{% if building.IsWonder %}showWonder{% else %}showBuilding{% endif %}('{{ building.Type }}'); return false;">{{ building.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.PrereqTech %}
                            <div class="summary">
                                <h3>Required Technology:</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        <li><a href="#" onclick="showTechnology('{{ item.PrereqTech.Type }}'); return false;">{{ item.PrereqTech.Name }}</a></li>
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.ObsoleteTech %}
                            <div class="summary">
                                <h3>{{ labels.get('obsoleteTechnologyHeader') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        <li><a href="#" onclick="showTechnology('{{ item.ObsoleteTech.Type }}'); return false;">{{ item.ObsoleteTech.Name }}</a></li>
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.LeadsToTechs and item.LeadsToTechs|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('leadsToTechLabel') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for tech in item.LeadsToTechs %}
                                        <li><a href="#" onclick="showTechnology('{{ tech.Type }}'); return false;">{{ tech.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.UnlockedUnits and item.UnlockedUnits|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('unitsUnlockedHeader') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for unit in item.UnlockedUnits %}
                                        <li><a href="#" onclick="showUnit('{{ unit.Type }}'); return false;">{{ unit.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.UnlockedBuildings and item.UnlockedBuildings|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('buildingsUnlockedLabel') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for building in item.UnlockedBuildings %}
                                        <li><a href="#" onclick="showBuilding('{{ building.Type }}'); return false;">{{ building.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.UnlockedWonders and item.UnlockedWonders|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('wondersUnlockedLabel') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for wonder in item.UnlockedWonders %}
                                        <li><a href="#" onclick="showWonder('{{ wonder.Type }}'); return false;">{{ wonder.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.UnlockedProjects and item.UnlockedProjects|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('projectsUnlockedHeader') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for project in item.UnlockedProjects %}
                                        <li><a href="#" onclick="showWonder('{{ project.Type }}'); return false;">{{ project.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.RevealedResources and item.RevealedResources|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('resourcesRevealedLabel') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for resource in item.RevealedResources %}
                                        <li><a href="#" onclick="showResource('{{ resource.Type }}'); return false;">{{ resource.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.WorkerActions and item.WorkerActions|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('workerActionLabel') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for action in item.WorkerActions %}
                                        <li>{% if action.CreatesImprovement %}<a href="#" onclick="showImprovement('{{ action.CreatesImprovement.Type }}'); return false;">{{ action.Name }}</a>{% else %}{{ action.Name }}{% endif %}</li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if item.FreePromotions and item.FreePromotions|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('freePromotionsHeader') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for promotion in item.FreePromotions %}
                                        <li><a href="#" onclick="showPromotion('{{ promotion.Type }}'); return false;">{{ promotion.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if active_page == "promotions" %}
                                {% if item.RequiredPromotions and item.RequiredPromotions|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('requiredPromotionsHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for promotion in item.RequiredPromotions %}
                                            <li><a href="#" onclick="showPromotion('{{ promotion.Type }}'); return false;">{{ promotion.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.LeadsToPromotions and item.LeadsToPromotions|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('leadsToPromotionsHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for promotion in item.LeadsToPromotions %}
                                            <li><a href="#" onclick="showPromotion('{{ promotion.Type }}'); return false;">{{ promotion.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                            {% endif %}
                            {% if item.Civilizations and item.Civilizations|length > 0 %}
                            <div class="summary">
                                <h3>{{ labels.get('civilizationHeader') }}</h3>
                                <div class="text-box">
                                    <ul class="item-list">
                                        {% for civ in item.Civilizations %}
                                        <li><a href="#" onclick="showCivilization('{{ civ.Type }}'); return false;">{{ civ.Name }}</a></li>
                                        {% endfor %}
                                    </ul>
                                </div>
                            </div>
                            {% endif %}
                            {% if active_page == "civilizations" %}
                                {% if item.Leaders and item.Leaders|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('leaderHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for leader in item.Leaders %}
                                            <li><a href="#" onclick="showCivilization('{{ leader.Type }}'); return false;">{{ leader.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.UniqueUnits and item.UniqueUnits|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('uniqueUnitsHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for unit in item.UniqueUnits %}
                                            <li><a href="#" onclick="showUnit('{{ unit.Type }}'); return false;">{{ unit.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.UniqueBuildings and item.UniqueBuildings|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('uniqueBuildingsHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for building in item.UniqueBuildings %}
                                            <li><a href="#" onclick="showBuilding('{{ building.Type }}'); return false;">{{ building.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.UniqueImprovements and item.UniqueImprovements|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('uniqueImprovementsHeader') }}</h3>
                                    <div class="text-box">
                                        <ul class="item-list">
                                            {% for improvement in item.UniqueImprovements %}
                                            <li><a href="#" onclick="showImprovement('{{ improvement.Type }}'); return false;">{{ improvement.Name }}</a></li>
                                            {% endfor %}
                                        </ul>
                                    </div>
                                </div>
                                {% endif %}
                                {% if item.Traits and item.Traits|length > 0 %}
                                <div class="summary">
                                    <h3>{{ labels.get('leaderTraitsHeader') }}</h3>
                                    <div class="text-box">
                                        {% for trait in item.Traits %}
                                        <div style="margin-bottom: 8px;">
                                            <strong>{{ trait.Name }}:</strong> {{ trait.Description | civ5format | safe }}
                                        </div>
                                        {% endfor %}
                                    </div>
                                </div>
                                {% endif %}
                            {% endif %}
                            {% if item.TextSections and item.TextSections|length > 0 %}
                                {% for section in item.TextSections %}
                                <div class="summary">
                                    <h3>{{ section.heading | civ5format | safe }}</h3>
                                    <div class="text-box">
                                        {{ section.text | civ5format | safe }}
                                    </div>
                                </div>
                                {% endfor %}
                            {% endif %}
                            {% if item.Strategy %}
                            <div class="summary">
                                <h3>{{ labels.get('strategyHeader') }}</h3>
                                <div class="text-box">
                                    {{ item.Strategy | civ5format | safe }}
                                </div>
                            </div>
                            {% endif %}
                            {% if item.Civilopedia %}
                            <div class="summary">
                                <h3>{{ labels.get('civilopediaHeader') }}</h3>
                                <div class="text-box">
                                    {{ item.Civilopedia | civ5format | safe }}
                                </div>
                            </div>
                            {% endif %}
                        </section>
                    </div>
                    {% endfor %}
                {% endfor %}
            </div>
        </main>

        <footer>
            <p>{{ civilopediaTitle }} - Version {{ version }}</p>
        </footer>
    </div>

    <script>
        function showHomePage() {
            // Update URL hash
            history.pushState(null, null, '#');

            // Hide all items
            const items = document.querySelectorAll('.article-detail');
            items.forEach(c => c.style.display = 'none');

            // Show placeholder
            const placeholder = document.querySelector('.placeholder');
            if (placeholder) placeholder.style.display = 'block';

            // Remove active state from all sidebar links
            const links = document.querySelectorAll('.sidebar-list a');
            links.forEach(link => link.classList.remove('active'));

            // Scroll to top of content
            document.getElementById('itemContent').scrollTop = 0;
        }

        function showItem(itemType) {
            // Update URL hash
            history.pushState(null, null, '#item-' + itemType);

            // Hide all items
            const items = document.querySelectorAll('.article-detail');
            items.forEach(c => c.style.display = 'none');

            // Hide placeholder
            const placeholder = document.querySelector('.placeholder');
            if (placeholder) placeholder.style.display = 'none';

            // Show selected item
            const selected = document.getElementById('item-' + itemType);
            if (selected) {
                selected.style.display = 'block';
                // Scroll to top of content
                document.getElementById('itemContent').scrollTop = 0;
            }

            // Update active state in sidebar with exact matching and expand containing section
            const links = document.querySelectorAll('.sidebar-list a');
            links.forEach(link => {
                const onclick = link.getAttribute('onclick');
                // Extract the item type from onclick="showItem('TYPE'); return false;"
                const match = onclick.match(/showItem\\('([^']+)'\\)/);
                if (match && match[1] === itemType) {
                    link.classList.add('active');

                    // Expand the parent section if it's collapsed
                    const parentSection = link.closest('.sidebar-section');
                    if (parentSection && parentSection.classList.contains('collapsed')) {
                        const list = parentSection.querySelector('.sidebar-list');
                        const header = parentSection.querySelector('.sidebar-section-header');
                        const icon = header ? header.querySelector('.section-icon') : null;

                        if (list) list.style.display = 'block';
                        if (icon) icon.textContent = '▼';
                        parentSection.classList.remove('collapsed');
                    }
                } else {
                    link.classList.remove('active');
                }
            });
        }

        function toggleSection(header) {
            const section = header.parentElement;
            const list = section.querySelector('.sidebar-list');
            const icon = header.querySelector('.section-icon');

            if (list.style.display === 'none') {
                list.style.display = 'block';
                icon.textContent = '▼';
                section.classList.remove('collapsed');
            } else {
                list.style.display = 'none';
                icon.textContent = '▶';
                section.classList.add('collapsed');
            }
        }

        // Handle browser back/forward buttons
        window.addEventListener('popstate', function() {
            handleHashChange();
        });

        // Handle URL hash on page load to show specific item
        window.addEventListener('DOMContentLoaded', function() {
            handleHashChange();
        });

        function handleHashChange() {
            const hash = window.location.hash;

            if (!hash || hash === '#') {
                showHomePage();
            } else if (hash.startsWith('#item-')) {
                const itemType = hash.substring(6); // Remove '#item-' prefix
                showItem(itemType);
            }
        }

        function showTechnology(techType) {
            window.location.href = 'technologies.html#item-' + techType;
        }

        function showPromotion(promotionType) {
            window.location.href = 'promotions.html#item-' + promotionType;
        }

        function showBuilding(buildingType) {
            // Check if the current page is buildings or wonders
            const currentPage = window.location.pathname.split('/').pop();
            if (currentPage === 'buildings.html' || currentPage === 'wonders.html') {
                // Stay on same page
                showItem(buildingType);
            } else {
                // Try buildings first, could be a wonder
                window.location.href = 'buildings.html#item-' + buildingType;
            }
        }

        function showUnit(unitType) {
            window.location.href = 'units.html#item-' + unitType;
        }

        function showWonder(wonderType) {
            window.location.href = 'wonders.html#item-' + wonderType;
        }

        function showResource(resourceType) {
            window.location.href = 'resources.html#item-' + resourceType;
        }

        function showImprovement(improvementType) {
            window.location.href = 'improvements.html#item-' + improvementType;
        }

        function showTerrain(terrainType) {
            window.location.href = 'terrain.html#item-' + terrainType;
        }

        function showTech(techType) {
            window.location.href = 'technologies.html#item-' + techType;
        }

        function showCivilization(techType) {
            window.location.href = 'civilizations.html#item-' + techType;
        }

        // Search functionality using global search index
        const searchInput = document.getElementById('globalSearch');
        const searchResults = document.getElementById('searchResults');
        let selectedIndex = -1;

        if (searchInput && window.globalSearchIndex) {
            searchInput.addEventListener('input', function(e) {
                const query = e.target.value.trim().toLowerCase();
                selectedIndex = -1; // Reset selection on new input

                if (query.length < 2) {
                    searchResults.style.display = 'none';
                    searchResults.innerHTML = '';
                    return;
                }

                // Search for matches in article titles across all categories
                const matches = window.globalSearchIndex.filter(article =>
                    article.name.toLowerCase().includes(query)
                ).slice(0, 10); // Limit to 10 results

                if (matches.length > 0) {
                    // Clear previous results
                    searchResults.innerHTML = '';

                    // Create and append each result element
                    matches.forEach(match => {
                        const resultDiv = document.createElement('div');
                        resultDiv.className = 'search-result-item';
                        resultDiv.setAttribute('data-page', match.page);
                        resultDiv.setAttribute('data-type', match.type);

                        const strong = document.createElement('strong');
                        strong.innerHTML = match.nameFormatted; // Use formatted name with HTML/colors
                        resultDiv.appendChild(strong);

                        // Add click handler
                        resultDiv.addEventListener('click', function() {
                            navigateToArticle(match.page, match.type);
                        });

                        searchResults.appendChild(resultDiv);
                    });
                    searchResults.style.display = 'block';
                } else {
                    searchResults.innerHTML = '<div class="search-result-item no-results">{{ noResultsFound }}</div>';
                    searchResults.style.display = 'block';
                }
            });

            // Keyboard navigation
            searchInput.addEventListener('keydown', function(e) {
                const items = searchResults.querySelectorAll('.search-result-item:not(.no-results)');

                if (items.length === 0) return;

                if (e.key === 'ArrowDown') {
                    e.preventDefault();
                    selectedIndex = Math.min(selectedIndex + 1, items.length - 1);
                    updateSelection(items);
                } else if (e.key === 'ArrowUp') {
                    e.preventDefault();
                    selectedIndex = Math.max(selectedIndex - 1, -1);
                    updateSelection(items);
                } else if (e.key === 'Enter') {
                    e.preventDefault();
                    if (selectedIndex >= 0 && selectedIndex < items.length) {
                        const selectedItem = items[selectedIndex];
                        const page = selectedItem.getAttribute('data-page');
                        const type = selectedItem.getAttribute('data-type');
                        navigateToArticle(page, type);
                    }
                } else if (e.key === 'Escape') {
                    searchResults.style.display = 'none';
                    selectedIndex = -1;
                }
            });

            function updateSelection(items) {
                items.forEach((item, index) => {
                    if (index === selectedIndex) {
                        item.classList.add('selected');
                        item.scrollIntoView({ block: 'nearest' });
                    } else {
                        item.classList.remove('selected');
                    }
                });
            }

            // Close search results when clicking outside
            document.addEventListener('click', function(e) {
                if (!e.target.closest('.search-container')) {
                    searchResults.style.display = 'none';
                    selectedIndex = -1;
                }
            });

            // Prevent closing when clicking inside search container
            searchInput.addEventListener('click', function(e) {
                e.stopPropagation();
            });
        }

        // Global keyboard shortcut: Press 'S' to focus search
        document.addEventListener('keydown', function(e) {
            // Check if 'S' key is pressed and search input is not already focused
            if (e.key === 's' || e.key === 'S') {
                // Don't trigger if user is typing in an input field
                if (document.activeElement.tagName === 'INPUT' ||
                    document.activeElement.tagName === 'TEXTAREA' ||
                    document.activeElement.isContentEditable) {
                    return;
                }
                e.preventDefault();
                searchInput.focus();
            }
        });

        function navigateToArticle(page, itemType) {
            const currentPage = window.location.pathname.split('/').pop();
            const targetPage = page + '.html';

            if (currentPage === targetPage) {
                // Same page, just show the item
                showItem(itemType);
                searchResults.style.display = 'none';
                searchInput.value = '';
            } else {
                // Different page, navigate
                window.location.href = targetPage + '#item-' + itemType;
            }
        }

        function escapeHtml(text) {
            const div = document.createElement('div');
            div.textContent = text;
            return div.innerHTML;
        }
    </script>
</body>
</html>
"""

# Concepts sidebar template (mimics in-game civilopedia layout)
# Removed - now using unified SIDEBAR_TEMPLATE




def convert_civ5_formatting(text):
    """Convert Civilization V formatting codes to HTML"""
    if not text:
        return text

    import re

    # Replace [NEWLINE] with <br>
    text = text.replace('[NEWLINE]', '<br>')
    text = text.replace('[TAB]', '&emsp;')

    # Remove LINK tags (they appear as [LINK=XXX]text[\LINK])
    text = re.sub(r'\[LINK=.*?\](.*?)\[\\LINK\]', r'\1', text)

    # Replace color tags
    text = re.sub(r'\[COLOR_POSITIVE_TEXT\](.*?)\[ENDCOLOR\]', r'<span class="positive">\1</span>', text)
    text = re.sub(r'\[COLOR_NEGATIVE_TEXT\](.*?)\[ENDCOLOR\]', r'<span class="negative">\1</span>', text)
    text = re.sub(r'\[COLOR_WARNING_TEXT\](.*?)\[ENDCOLOR\]', r'<span class="warning">\1</span>', text)
    text = re.sub(r'\[COLOR_CYAN\](.*?)\[ENDCOLOR\]', r'<span class="cyan">\1</span>', text)
    text = re.sub(r'\[COLOR_YELLOW\](.*?)\[ENDCOLOR\]', r'<span class="yellow">\1</span>', text)
    text = re.sub(r'\[COLOR_GREEN\](.*?)\[ENDCOLOR\]', r'<span class="green">\1</span>', text)
    text = re.sub(r'\[COLOR_MAGENTA\](.*?)\[ENDCOLOR\]', r'<span class="magenta">\1</span>', text)
    text = re.sub(r'\[COLOR_WHITE\](.*?)\[ENDCOLOR\]', r'<span class="white">\1</span>', text)
    text = re.sub(r'\[COLOR_PLAYER_DARK_GREEN_TEXT\](.*?)\[ENDCOLOR\]', r'<span class="player-dark-green">\1</span>', text)
    text = re.sub(r'\[COLOR_YIELD_FOOD\](.*?)\[ENDCOLOR\]', r'<span class="yield-food">\1</span>', text)
    text = re.sub(r'\[COLOR_YIELD_PRODUCTION\](.*?)\[ENDCOLOR\]', r'<span class="yield-production">\1</span>', text)
    text = re.sub(r'\[COLOR_YIELD_GOLD\](.*?)\[ENDCOLOR\]', r'<span class="yield-gold">\1</span>', text)
    text = re.sub(r'\[COLOR_YIELD_SCIENCE\](.*?)\[ENDCOLOR\]', r'<span class="yield-science">\1</span>', text)
    text = re.sub(r'\[COLOR_YIELD_CULTURE\](.*?)\[ENDCOLOR\]', r'<span class="yield-culture">\1</span>', text)
    text = re.sub(r'\[COLOR_YIELD_FAITH\](.*?)\[ENDCOLOR\]', r'<span class="yield-faith">\1</span>', text)

    # Replace icon tags with emoji/text equivalents
    text = text.replace('[ICON_PRODUCTION]', '🔨')
    text = text.replace('[ICON_FOOD]', '🌾')
    text = text.replace('[ICON_GOLD]', '💰')
    text = text.replace('[ICON_RESEARCH]', '⚗️')
    text = text.replace('[ICON_CULTURE]', '🎵')
    text = text.replace('[ICON_PEACE]', '🕊️')
    text = text.replace('[ICON_FAITH]', '🕊️')
    text = text.replace('[ICON_TOURISM]', '🗿')
    text = text.replace('[ICON_HAPPINESS_1]', '😊')
    text = text.replace('[ICON_UNHAPPINESS]', '😞')
    text = text.replace('[ICON_STRENGTH]', '⚔️')
    text = text.replace('[ICON_MOVES]', '👣')
    text = text.replace('[ICON_RANGE_STRENGTH]', '🏹')
    text = text.replace('[ICON_GREAT_PEOPLE]', '⭐')
    text = text.replace('[ICON_GOLDEN_AGE]', '🌟')
    text = text.replace('[ICON_CAPITAL]', '🏛️')
    text = text.replace('[ICON_TRADE]', '💱')
    text = text.replace('[ICON_CONNECTED]', '🔗')
    text = text.replace('[ICON_BULLET]', '• ')
    text = text.replace('[ICON_PUPPET]', '🎭')
    text = text.replace('[ICON_SPY]', '🕵️')
    text = text.replace('[ICON_RES_MARRIAGE]', '💍')
    text = text.replace('[ICON_TROPHY_GOLD]', '🥇')
    text = text.replace('[ICON_TROPHY_SILVER]', '🥈')
    text = text.replace('[ICON_TROPHY_BRONZE]', '🥉')

    # Replace resource icons with text
    text = re.sub(r'\[ICON_RES_([A-Z_]+)\]', '', text)

    # Remove any remaining unknown tags
    text = re.sub(r'\[ICON_[A-Z_0-9]+\]', '', text)

    # Remove standalone color tags (not paired with ENDCOLOR)
    text = re.sub(r'\[COLOR_[A-Z_]+\]', '', text)
    text = text.replace('[ENDCOLOR]', '')

    return text


def format_yield(value):
    """Format yield value, removing trailing zeros and the decimal point if no decimals remain.
    """
    if isinstance(value, (int, float)):
        # Format with up to 2 decimal places
        formatted = f"{value:.2f}"
        # Remove trailing zeros and decimal point if not needed
        formatted = formatted.rstrip('0').rstrip('.')
        return formatted
    return str(value)


def group_adjacency_bonuses(bonuses):
    """Group adjacency bonuses by their yield combinations.

    Groups bonuses with identical yields together, merging their descriptions.
    Returns a list of grouped bonuses in the format:
    [
        {
            'Description': 'Lumber Mill/Manufactory',
            'Yields': [
                {'Yield': 0.5, 'YieldName': '🔨 Production'},
                {'Yield': 0.5, 'YieldName': '💰 Gold'}
            ]
        },
        ...
    ]
    """
    if not bonuses:
        return []

    # Create a signature for each bonus based on its yields
    # Group by improvement/terrain, then collect all yields for that improvement
    from collections import defaultdict

    # First, group by Description to collect all yields for each improvement/terrain
    by_description = defaultdict(list)
    for bonus in bonuses:
        by_description[bonus['Description']].append({
            'Yield': bonus['Yield'],
            'YieldName': bonus['YieldName'],
            'YieldType': bonus.get('YieldType', '')
        })

    # Now group descriptions that have identical yield sets
    # Create a tuple key from the sorted yields for comparison
    def yield_key(yields_list):
        # Sort by YieldType to ensure consistent ordering
        sorted_yields = sorted(yields_list, key=lambda y: y.get('YieldType', ''))
        return tuple((y['Yield'], y.get('YieldType', '')) for y in sorted_yields)

    # Group by yield signature
    grouped = defaultdict(list)
    for description, yields_list in by_description.items():
        key = yield_key(yields_list)
        grouped[key].append({
            'Description': description,
            'Yields': yields_list
        })

    # Merge descriptions with identical yields
    result = []
    for yield_sig, items in grouped.items():
        # Merge all descriptions
        merged_description = '/'.join(item['Description'] for item in items)
        # Get yields from the first item (they're all the same)
        yields = items[0]['Yields']
        result.append({
            'Description': merged_description,
            'Yields': yields
        })

    return result


def build_atlas_lookup(atlas_metadata):
    """Build a lookup dictionary from atlas metadata.

    Args:
        atlas_metadata: List of atlas info from exported JSON

    Returns:
        Dictionary mapping atlas name to {filename, rows, columns}
    """
    atlas_lookup = {}

    for atlas_info in atlas_metadata:
        atlas_name = atlas_info['Atlas']
        atlas_lookup[atlas_name] = {
            'filename': atlas_info['Filename'],
            'rows': int(atlas_info['IconsPerColumn']),
            'columns': int(atlas_info['IconsPerRow']),
            'icon_size': int(atlas_info['IconSize'])
        }

    return atlas_lookup


def is_dds_uncompressed(dds_path):
    """Check if a DDS file uses uncompressed format (no FourCC).

    Returns True if uncompressed, False if compressed (DXT1/3/5, BC7, etc.)
    """
    import struct
    try:
        with open(dds_path, 'rb') as f:
            magic = f.read(4)
            if magic != b'DDS ':
                return False
            header = f.read(124)
            # Pixel format starts at offset 72 in header
            pf_flags = struct.unpack('<I', header[76:80])[0]
            fourcc = header[80:84]
            # DDPF_FOURCC = 0x4, DDPF_RGB = 0x40
            # If no FourCC and has RGB flag, it's uncompressed
            return fourcc == b'\x00\x00\x00\x00' and (pf_flags & 0x40)
    except:
        return False

def load_indexed_dds(dds_path):
    """Load a DDS file that has an associated index file for de-swizzling.

    Some Civ5 DDS files use a virtual texture system where the pixel data
    is stored in a scrambled order and an index file maps blocks to their
    correct positions.

    """


    # Check for index file (try multiple case variations)
    stem = dds_path.stem
    possible_names = [
        f"{stem}-index.dds",
        f"{stem.lower()}-index.dds",
        f"{stem.upper()}-index.dds",
    ]

    index_path = None
    for name in possible_names:
        candidate = dds_path.parent / name
        if candidate.exists():
            index_path = candidate
            break

    if index_path is None:
        return None

    #try:
    # Load index and get its dimensions from header
    with open(index_path, 'rb') as f:
        f.read(4)
        idx_header = f.read(124)
        idx_height = struct.unpack('<I', idx_header[8:12])[0]
        idx_width = struct.unpack('<I', idx_header[12:16])[0]
        idx_data = f.read()

        index = np.frombuffer(idx_data, dtype=np.uint16).reshape(idx_height, idx_width)

    # Load main texture and get dimensions from header
    with open(dds_path, 'rb') as f:
        f.read(4)
        header = f.read(124)
        height = struct.unpack('<I', header[8:12])[0]
        width = struct.unpack('<I', header[12:16])[0]
        main_data = np.frombuffer(f.read(), dtype=np.uint8)

    # Determine block size based on index content
    # Try largest block size first that has enough blocks for the max index
    max_block_idx = np.max(index)
    needed_blocks = max_block_idx + 1

    block_size = None
    for b in [8, 4]:  # Try larger first
        if width % b == 0 and height % b == 0:
            available_blocks = (width // b) * (height // b)
            if available_blocks >= needed_blocks:
                block_size = b
                break

    if block_size is None:
        print(f"  Skipping indexed loading: cannot determine valid block size for {width}x{height}")
        return None

    # Read source as proper image
    src_pixels = main_data[:width*height*4].reshape(height, width, 4)

    # Calculate blocks per row in source
    blocks_per_row = width // block_size

    out_height = idx_height * block_size
    out_width = idx_width * block_size
    output = np.zeros((out_height, out_width, 4), dtype=np.uint8)

    # Map blocks using index
    for iy in range(idx_height):
        for ix in range(idx_width):
            block_idx = index[iy, ix]
            src_bx = block_idx % blocks_per_row
            src_by = block_idx // blocks_per_row

            src_y = src_by * block_size
            src_x = src_bx * block_size

            output[iy*block_size:(iy+1)*block_size, ix*block_size:(ix+1)*block_size] = src_pixels[src_y:src_y+block_size, src_x:src_x+block_size]

    # Convert BGRA to RGBA
    return PILImage.fromarray(output[:,:,[2,1,0,3]], 'RGBA')

    #except Exception as e:
    #    print(f"  Failed to load indexed DDS: {e}")
    #    return None

def extract_or_find_icon_from_atlas(atlas_name, atlas_lookup, portrait_index, icon_name):
    """Extract a single icon from a DDS atlas.

    Returns:
        Relative path to the extracted icon, or None if extraction failed
    """

    if atlas_name not in atlas_lookup:
        print(f"  Warning: Atlas '{atlas_name}' not found in metadata")
        return None

    atlas_info = atlas_lookup[atlas_name]
    filename = atlas_info['filename']
    atlas_folder = Path(filename).stem

    # check if the icon has already been exported
    icon_filename = f"{icon_name}.png"

    output_folder = None
    if os.path.exists(Path(BNW_ICONS_FOLDER) / atlas_folder):
        output_folder = BNW_ICONS_FOLDER
    elif os.path.exists(Path(GENERATED_ICONS_FOLDER) / atlas_folder):
        output_folder = GENERATED_ICONS_FOLDER

    if output_folder:
        icon_path = Path(output_folder) / atlas_folder / icon_filename
        if os.path.exists(str(icon_path)):
            return f"{output_folder}/{atlas_folder}/{icon_filename}"

    rows = atlas_info['rows']
    columns = atlas_info['columns']
    icon_size = atlas_info['icon_size']

    base_paths = ATLAS_BASE_PATHS

    if Path(BNW_ATLASES_FOLDER).exists():
        base_paths.insert(0, BNW_ATLASES_FOLDER)
        
    # Search for the DDS file in base paths
    dds_path = None
    for base_path in base_paths:
        is_bnw_icon = base_path == BNW_ATLASES_FOLDER
        base_path_ = Path(base_path)
        dds_path = base_path_ / filename
        if not dds_path.exists():
            # Try without subdirectory (flatten the path)
            flat_filename = Path(filename).name
            dds_path = base_path_ / flat_filename
            if not dds_path.exists():
                # Try searching in all subdirectories
                if base_path_.exists() and base_path_.is_dir():
                    for subdir in base_path_.iterdir():
                        if subdir.is_dir():
                            dds_path = subdir / flat_filename
                            if dds_path.exists():
                                break

        if dds_path.exists():
            break

    if not dds_path or not dds_path.exists():
        print(f"Warning: Atlas {atlas_name} ({filename}) not found")
        return None
    
    print(f"Exporting index {portrait_index} from atlas {filename}: {icon_name}")

    # Calculate position in grid (row-major order)
    row = portrait_index // columns
    col = portrait_index % columns

    # Validate the index is within bounds
    if row >= rows or col >= columns:
        print(f"  Warning: Icon index {portrait_index} out of bounds for atlas {filename} ({columns}x{rows})")
        return None

    # Prepare output path
    output_path = BNW_ICONS_FOLDER if is_bnw_icon else GENERATED_ICONS_FOLDER
    atlas_folder = Path(output_path) / filename
    atlas_folder.mkdir(parents=True, exist_ok=True)
    

    # Try loading indexed DDS (virtual texture format with -index.dds file)
    atlas_img = load_indexed_dds(dds_path)
    if atlas_img:
        width, height = atlas_img.size

        left = col * icon_size
        top = row * icon_size

        icon = atlas_img.crop((left, top, left + icon_size, top + icon_size))
        icon.save(str(icon_path))

        return f"{output_folder}/{atlas_folder}/{icon_filename}"

    with WandImage(filename=str(dds_path)) as atlas:
        width, height = atlas.width, atlas.height
        icon_width = width // columns
        icon_height = height // rows

        left = col * icon_size
        top = row * icon_size

        with atlas.clone() as icon:
            icon.crop(left, top, left + icon_size, top + icon_size)
            icon.save(filename=str(icon_path))

    return f"{output_folder}/{atlas_folder}/{icon_filename}"


def process_icons(data):
    """Process all icons from the exported data.

    Args:
        data: The exported JSON data
        output_path: Base output directory for the website

    Returns:
        Dictionary mapping item Type to icon path
    """

    print("\nProcessing icons...")

    # Build atlas lookup from metadata
    atlas_metadata = data.get('metadata', {}).get('iconAtlases', [])
    if not atlas_metadata:
        print("  Warning: No icon atlas metadata found in export data")
        return {}

    atlas_lookup = build_atlas_lookup(atlas_metadata)
    print(f"  Loaded metadata for {len(atlas_lookup)} atlases")

    icon_map = {}

    # Categories to process
    categories = [
        'promotions', 'units', 'buildings', 'wonders', 'technologies',
        'policies', 'beliefs', 'religions', 'resources', 'improvements',
        'terrain', 'civilizations', 'leaders', 'corporations'
    ]

    for category in categories:
        items = data.get(category, [])
        if isinstance(items, dict):  # Handle nested structure like greatPeople
            items = []
            for subkey in ['specialists', 'greatPersons']:
                items.extend(data[category].get(subkey, []))

        if not items:
            continue

        print(f"  Processing {len(items)} {category}...")

        for item in items:
            atlas_name = item.get('IconAtlas')
            portrait_index = item.get('PortraitIndex')
            item_type = item.get('Type')

            if not atlas_name or portrait_index is None or not item_type:
                continue

            # Extract the icon
            icon_path = extract_or_find_icon_from_atlas(atlas_name, atlas_lookup, portrait_index, item_type)
            if icon_path:
                icon_map[item_type] = icon_path

    return icon_map


def create_templates_env():
    """Create a Jinja2 environment with string templates"""
    from jinja2 import BaseLoader, Environment

    class StringLoader(BaseLoader):
        def __init__(self, templates):
            self.templates = templates

        def get_source(self, environment, template):
            if template in self.templates:
                source = self.templates[template]
                return source, None, lambda: True
            raise Exception(f"Template {template} not found")

    templates = {
        'index.html': INDEX_TEMPLATE,
        'sidebar.html': SIDEBAR_TEMPLATE,
        'concepts.html': SIDEBAR_TEMPLATE,
        'civilizations_list.html': SIDEBAR_TEMPLATE,
    }

    env = Environment(
        loader=StringLoader(templates),
        autoescape=select_autoescape(),
        trim_blocks=True,      # Remove first newline after block
        lstrip_blocks=True     # Remove leading whitespace before blocks
    )

    # Add custom filters
    env.filters['civ5format'] = convert_civ5_formatting
    env.filters['format_yield'] = format_yield
    env.filters['group_adjacency_bonuses'] = group_adjacency_bonuses

    return env


def generate_html(json_file):
    """Generate HTML files from JSON export"""

    # Load JSON data
    print(f"Loading data from {json_file}...")
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)

    # Create output directory (remove existing files first)
    output_path = Path(OUTPUT_FOLDER)
    if output_path.exists():
        print(f"Cleaning existing output directory: {output_path}")
        shutil.rmtree(output_path)
    output_path.mkdir(exist_ok=True)

    bnw_icons_path = Path(BNW_ICONS_FOLDER)
    bnw_icons_path.mkdir(exist_ok=True)

    gen_icons_path = Path(GENERATED_ICONS_FOLDER)
    gen_icons_path.mkdir(exist_ok=True)

    # Create CSS directory
    css_path = output_path / 'css'
    css_path.mkdir(exist_ok=True)

    # Write CSS file
    print("Creating CSS...")
    with open(css_path / 'style.css', 'w', encoding='utf-8') as f:
        f.write(CSS_TEMPLATE)

    # Process icons from DDS atlases
    
    icon_map = process_icons(data)

    # Add IconPath to all items based on icon_map
    def add_icon_paths(items):
        """Add IconPath field to items based on their Type"""
        if not items:
            return items
        if isinstance(items, dict):
            # Handle nested structures
            for key, value in items.items():
                add_icon_paths(value)
            return items
        if not isinstance(items, list):
            return items
        for item in items:
            if isinstance(item, dict) and 'Type' in item:
                item_type = item['Type']
                if item_type in icon_map:
                    item['IconPath'] = icon_map[item_type]
        return items

    # Add icon paths to all categories
    for category in ['promotions', 'units', 'buildings', 'wonders', 'technologies',
                     'policies', 'beliefs', 'religions', 'resources', 'improvements',
                     'terrain', 'civilizations', 'leaders', 'corporations', 'concepts',
                     'cityStates', 'resolutions', 'leagueProjects', 'specialProjects']:
        if category in data:
            add_icon_paths(data[category])
    # Handle nested greatPeople structure
    if 'greatPeople' in data:
        add_icon_paths(data['greatPeople'])

    # Create Jinja2 environment
    env = create_templates_env()

    # Generate index page
    print("Generating index page...")
    template = env.get_template('index.html')
    localizedLabels = data.get('metadata', {}).get('localizedLabels', {})
    html = template.render(
        version=CIVILOPEDIA_VERSION,
        civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
        searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
        noResultsFound=LOC['NO_RESULTS_FOUND'],
        welcomeMessage=LOC['WELCOME_MESSAGE'],
        currentVersion=LOC['CURRENT_VERSION'],
        helpText=localizedLabels.get('homePageHelpText'),
        quoteText=localizedLabels.get('homePageBlurbText'),
        metadata=data['metadata'],
        labels=localizedLabels
    )
    with open(output_path / 'index.html', 'w', encoding='utf-8') as f:
        f.write(html)
 
    # Generate game concepts with sidebar layout
    if 'concepts' in data and len(data['concepts']) > 0:
        print(f"Generating game concepts page with {len(data['concepts'])} concepts...")

        # Group concepts by header using the localized names
        from collections import defaultdict
        concepts_by_header = defaultdict(list)
        for concept in data['concepts']:
            localized_name = concept.get('HeaderName')
            if localized_name and localized_name.strip():
                concepts_by_header[localized_name].append(concept)

        # Apply dependency sorting to each header group
        def dependency_sort(concepts):
            """Sort concepts based on InsertBefore and InsertAfter dependencies (topological sort)

            This matches the logic in CivilopediaScreen.lua
            """
            if not concepts:
                return concepts

            # First, assign InsertAfter to items without any insert directive
            # to maintain original order
            for i in range(len(concepts) - 1, -1, -1):
                concept = concepts[i]
                if not concept.get('InsertBefore') and not concept.get('InsertAfter'):
                    # Look backwards for a previous concept without dependencies
                    for j in range(i - 1, -1, -1):
                        prev_concept = concepts[j]
                        if not prev_concept.get('InsertBefore') and not prev_concept.get('InsertAfter'):
                            concept['InsertAfter'] = prev_concept['Type']
                            break

            # Build index by Type
            articles_by_type = {c['Type']: c for c in concepts}
            # Use Type strings as keys since dicts aren't hashable in Python
            dependencies = {c['Type']: set() for c in concepts}

            # Build dependency graph
            for concept in concepts:
                concept_type = concept['Type']

                insert_before = concept.get('InsertBefore')
                if insert_before and insert_before in articles_by_type:
                    # If this concept should be inserted before another,
                    # then the other article depends on this one
                    dependencies[insert_before].add(concept_type)

                insert_after = concept.get('InsertAfter')
                if insert_after and insert_after in articles_by_type:
                    # If this concept should be inserted after another,
                    # then this concept depends on the other
                    dependencies[concept_type].add(insert_after)

            # Topological sort
            sorted_list = []
            article_count = len(concepts)

            while len(sorted_list) < article_count:
                # Find a node with 0 dependencies
                article = None
                for concept in concepts:
                    concept_type = concept['Type']
                    if concept_type in dependencies and len(dependencies[concept_type]) == 0:
                        article = concept
                        break

                if article is None:
                    # Dependency cycle detected
                    print("Warning: Failed to sort concepts topologically! Dependency cycles detected.")
                    return concepts
                else:
                    # Insert node
                    sorted_list.append(article)

                    # Remove node
                    article_type = article['Type']
                    del dependencies[article_type]
                    for concept_type in dependencies:
                        dependencies[concept_type].discard(article_type)

            return sorted_list

        # Sort each header group
        for header in concepts_by_header:
            concepts_by_header[header] = dependency_sort(concepts_by_header[header])

        # Create single-page layout with sidebar navigation
        template = env.get_template('concepts.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='concepts',
            title=localizedLabels.get('civilopediaCategoryGameConcepts'),
            pageLabel=localizedLabels.get('gameConceptPageLabel'),
            helpText=localizedLabels.get('gameConceptsHelpText'),
            quoteText=localizedLabels.get('gameConceptsQuote'),
            grouped_items=[(header, concepts) for header, concepts in concepts_by_header.items()],
            labels=localizedLabels
        )
        with open(output_path / 'concepts.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate civilizations page with sidebar (includes both civilizations and leaders)
    civ_count = len(data.get('civilizations', []))
    leader_count = len(data.get('leaders', []))
    print(f"Generating civilizations page with {civ_count} civilizations and {leader_count} leaders...")
    template = env.get_template('sidebar.html')

    # Group civilizations and leaders together
    grouped_civs_leaders = [
        (localizedLabels.get('civilizationsSection1'), sorted(data.get('civilizations', []), key=lambda x: x['Name'])),
        (localizedLabels.get('civilizationsSection2'), sorted(data.get('leaders', []), key=lambda x: x['Name']))
    ]

    html = template.render(
        version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
        active_page='civilizations',
        title=localizedLabels.get('civilopediaCategoryCivilizations'),
        pageLabel=localizedLabels.get('civilizationsPageLabel'),
        helpText=localizedLabels.get('civilizationsHelpText'),
        quoteText=localizedLabels.get('civilizationsQuote'),
        grouped_items=grouped_civs_leaders,
        labels=localizedLabels
    )
    with open(output_path / 'civilizations.html', 'w', encoding='utf-8') as f:
        f.write(html)

    # Generate buildings with era grouping and sidebar
    if 'buildings' in data and len(data['buildings']) > 0:
        print(f"Generating buildings page with {len(data['buildings'])} buildings...")

        # Group buildings by era
        from collections import defaultdict
        buildings_by_era = defaultdict(list)
        for building in data['buildings']:
            # Skip wonders (they have their own page)
            if building.get('IsWonder') or building.get('IsNationalWonder'):
                continue
            era_name = building.get('EraName', 'Unknown Era')
            buildings_by_era[era_name].append(building)

        # Sort eras by the first building's Era ID (to maintain chronological order)
        era_order = {}
        for building in data['buildings']:
            if building.get('IsWonder') or building.get('IsNationalWonder'):
                continue
            era_name = building.get('EraName', 'Unknown Era')
            if era_name not in era_order:
                era_order[era_name] = building.get('EraID', 999)  # Use Era ID as sort key

        sorted_eras = sorted(buildings_by_era.items(), key=lambda x: era_order[x[0]])

        # Create sidebar page
        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='buildings',
            title=localizedLabels.get('civilopediaCategoryBuildings'),
            pageLabel=localizedLabels.get('buildingsPageLabel'),
            helpText=localizedLabels.get('buildingsHelpText'),
            quoteText=localizedLabels.get('buildingsQuote'),
            grouped_items=[(era, sorted(buildings, key=lambda x: x['Name'])) for era, buildings in sorted_eras],
            labels=localizedLabels
        )
        with open(output_path / 'buildings.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate technologies with era grouping and sidebar
    if 'technologies' in data and len(data['technologies']) > 0:
        print(f"Generating technologies page with {len(data['technologies'])} technologies...")

        # Group technologies by era
        from collections import defaultdict
        tech_by_era = defaultdict(list)
        for tech in data['technologies']:
            era_name = tech.get('EraName', 'Unknown Era')
            tech_by_era[era_name].append(tech)

        # Sort eras by the first tech's Era ID (to maintain chronological order)
        era_order = {}
        for tech in data['technologies']:
            era_name = tech.get('EraName', 'Unknown Era')
            if era_name not in era_order:
                era_order[era_name] = tech.get('EraID', 'ZZZ')  # Use Era ID as sort key

        sorted_eras = sorted(tech_by_era.items(), key=lambda x: era_order[x[0]])

        # Create sidebar page
        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='technologies',
            title=localizedLabels.get('civilopediaCategoryTechnologies'),
            pageLabel=localizedLabels.get('technologiesPageLabel'),
            helpText=localizedLabels.get('technologiesHelpText'),
            quoteText=localizedLabels.get('technologiesQuote'),
            grouped_items=[(era, sorted(techs, key=lambda x: x['Name'])) for era, techs in sorted_eras],
            labels=localizedLabels
        )
        with open(output_path / 'technologies.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate policies with branch grouping and sidebar
    if 'policies' in data and len(data['policies']) > 0:
        print(f"Generating policies page with {len(data['policies'])} policies...")

        # Group policies by branch
        from collections import defaultdict
        policy_by_branch = defaultdict(list)
        for policy in data['policies']:
            branch_name = policy.get('PolicyBranchName')
            if branch_name:  # Skip policies without a branch
                policy_by_branch[branch_name].append(policy)

        policy_order = {}
        for policy in data['policies']:
            branch_name = policy.get('PolicyBranchName')
            if branch_name not in policy_order:
                policy_order[branch_name] = policy.get('PolicyBranchID', 'ZZZ')  # Use Policy Branch ID as sort key


        sorted_branches = sorted(policy_by_branch.items(), key=lambda x: policy_order[x[0]])

        # Sort policies within each branch: openers first, then alphabetically by name
        def sort_policies(policies):
            return sorted(policies, key=lambda x: (not x.get('IsOpener', False), x['Name']))

        # Create sidebar page
        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='policies',
            title=localizedLabels.get('civilopediaCategoryPolicies'),
            pageLabel=localizedLabels.get('policiesPageLabel'),
            helpText=localizedLabels.get('policiesHelpText'),
            quoteText=localizedLabels.get('policiesQuote'),
            grouped_items=[(branch, sort_policies(policies)) for branch, policies in sorted_branches],
            labels=localizedLabels
        )
        with open(output_path / 'policies.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate wonders with branch grouping and sidebar
    if 'wonders' in data and len(data['wonders']) > 0:
        print(f"Generating wonders page with {len(data['wonders'])} wonders...")

        # Group wonders by branch
        from collections import defaultdict
        wonder_by_category = defaultdict(list)
        category_order = {}
        category_order[data.get('metadata', {}).get('localizedLabels', {}).get('wonderSection1')] = 1 # world wonders
        category_order[data.get('metadata', {}).get('localizedLabels', {}).get('wonderSection2')] = 2 # national wonders
        category_order[data.get('metadata', {}).get('localizedLabels', {}).get('wonderSection3')] = 3 # projects
        category_order[data.get('metadata', {}).get('localizedLabels', {}).get('wonderSection4')] = 4 # corporation HQs

        for wonder in data['wonders']:
            if wonder.get('IsCorporation'):
                category = data.get('metadata', {}).get('localizedLabels', {}).get('wonderSection4')
            elif wonder.get('IsNationalWonder'):
                category = data.get('metadata', {}).get('localizedLabels', {}).get('wonderSection2')
            elif wonder.get('IsProject'):
                category = data.get('metadata', {}).get('localizedLabels', {}).get('wonderSection3')
            else:
                category = data.get('metadata', {}).get('localizedLabels', {}).get('wonderSection1')
            wonder_by_category[category].append(wonder)

        sorted_categories = sorted(wonder_by_category.items(), key=lambda x: category_order[x[0]])

        # Create sidebar page
        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='wonders',
            title=localizedLabels.get('civilopediaCategoryWonders'),
            pageLabel=localizedLabels.get('wondersPageLabel'),
            helpText=localizedLabels.get('wondersHelpText'),
            quoteText=localizedLabels.get('wondersQuote'),
            grouped_items=[(category, sorted(wonder, key=lambda x: x['Name'])) for category, wonder in sorted_categories],
            labels=localizedLabels
        )
        with open(output_path / 'wonders.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate units with era grouping and sidebar
    if 'units' in data and len(data['units']) > 0:
        print(f"Generating units page with {len(data['units'])} units...")

        # Group units by era
        from collections import defaultdict
        units_by_era = defaultdict(list)
        for unit in data['units']:
            era_name = unit.get('EraName', 'Unknown Era')
            units_by_era[era_name].append(unit)

        # Sort eras by the first unit's Era ID (to maintain chronological order)
        era_order = {}
        for unit in data['units']:
            era_name = unit.get('EraName', 'Unknown Era')
            if era_name not in era_order:
                era_order[era_name] = unit.get('EraID', 999)  # Use Era ID as sort key

        sorted_eras = sorted(units_by_era.items(), key=lambda x: era_order[x[0]])

        # Create sidebar page
        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='units',
            title=localizedLabels.get('civilopediaCategoryUnits'),
            pageLabel=localizedLabels.get('unitsPageLabel'),
            helpText=localizedLabels.get('unitsHelpText'),
            quoteText=localizedLabels.get('unitsQuote'),
            grouped_items=[(era, sorted(units, key=lambda x: x['Name'])) for era, units in sorted_eras],
            labels=localizedLabels
        )
        with open(output_path / 'units.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate beliefs page with religion categorization
    if 'beliefs' in data and len(data['beliefs']) > 0:
        print(f"Generating beliefs page with {len(data['beliefs'])} beliefs and {len(data.get('religions', []))} religions...")

        from collections import defaultdict
        beliefs_by_category = defaultdict(list)

        # Add religions as first category
        if 'religions' in data and len(data['religions']) > 0:
            religions_category = localizedLabels.get('religionsCategory')
            beliefs_by_category[religions_category] = sorted(data['religions'], key=lambda x: x['Name'])

        # Group beliefs by category
        for belief in data['beliefs']:
            category_name = belief.get('CategoryName', 'Unknown')
            beliefs_by_category[category_name].append(belief)

        # Define category order
        category_order = {
            localizedLabels.get('religionsCategory'): 0,
            localizedLabels.get('pantheonCategory'): 1,
            localizedLabels.get('founderCategory'): 2,
            localizedLabels.get('followerCategory'): 3,
            localizedLabels.get('enhancerCategory'): 4,
            localizedLabels.get('reformationCategory'): 5,
        }

        sorted_categories = sorted(
            beliefs_by_category.items(),
            key=lambda x: category_order.get(x[0], 999)
        )

        # Sort items within each category safely
        grouped_items_sorted = []
        for category, items in sorted_categories:
            sorted_items = sorted(items, key=lambda x: x.get('Name', ''))
            grouped_items_sorted.append((category, sorted_items))

        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='beliefs',
            title=localizedLabels.get('civilopediaCategoryReligion'),
            pageLabel=localizedLabels.get('beliefsPageLabel'),
            helpText=localizedLabels.get('beliefsHelpText'),
            quoteText=localizedLabels.get('beliefsQuote'),
            grouped_items=grouped_items_sorted,
            labels=localizedLabels
        )
        with open(output_path / 'beliefs.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate promotions with category grouping and sidebar
    if 'promotions' in data and len(data['promotions']) > 0:
        print(f"Generating promotions page with {len(data['promotions'])} promotions...")

        # Group promotions by PediaType (category)
        from collections import defaultdict
        promotions_by_category = defaultdict(list)
        # Track the label for each category
        category_labels = {}
        for promotion in data['promotions']:
            pedia_type = promotion.get('PediaType')
            pedia_type_label = promotion.get('PediaTypeLabel')
            if pedia_type:  # Skip promotions without a category
                promotions_by_category[pedia_type].append(promotion)
                # Store the label for this category
                if pedia_type not in category_labels and pedia_type_label:
                    category_labels[pedia_type] = pedia_type_label

        # Define the category order from CivilopediaScreen.lua
        category_order = [
            "PEDIA_MELEE",
            "PEDIA_MOUNTED",
            "PEDIA_RANGED",
            "PEDIA_SIEGE",
            "PEDIA_NAVAL",
            "PEDIA_CARSUB",
            "PEDIA_AIR",
            "PEDIA_SCOUTING",
            "PEDIA_DIPLO",
            "PEDIA_CIVILIAN",
            "PEDIA_HEAL",
            "PEDIA_SHARED",
            "PEDIA_ATTRIBUTES",
            "PEDIA_BARBARIAN"
        ]

        # Sort categories by the defined order, using localized labels
        sorted_categories = []
        for category in category_order:
            if category in promotions_by_category:
                label = category_labels.get(category)
                sorted_categories.append((label, promotions_by_category[category]))

        # Sort promotions within each category alphabetically by name
        sorted_categories = [(label, sorted(promos, key=lambda x: x['Name'])) for label, promos in sorted_categories]

        # Create sidebar page
        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='promotions',
            title=localizedLabels.get('civilopediaCategoryPromotions'),
            pageLabel=localizedLabels.get('promotionsPageLabel'),
            helpText=localizedLabels.get('promotionsHelpText'),
            quoteText=localizedLabels.get('promotionsQuote'),
            grouped_items=sorted_categories,
            labels=localizedLabels
        )
        with open(output_path / 'promotions.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate resources with type grouping and sidebar
    if 'resources' in data and len(data['resources']) > 0:
        print(f"Generating resources page with {len(data['resources'])} resources...")

        # Group resources by ResourceUsage (0=Bonus, 1=Luxury, 2=Strategic)
        from collections import defaultdict
        resources_by_type = defaultdict(list)
        for resource in data['resources']:
            resource_usage = resource.get('ResourceUsage', 0)
            resources_by_type[resource_usage].append(resource)

        # Define the type order and names
        type_order = [
            (0, localizedLabels.get('bonusResourcesCategory')),
            (1, localizedLabels.get('luxuryResourcesCategory')),
            (2, localizedLabels.get('strategicResourcesCategory'))
        ]

        # Sort resources by type
        sorted_types = []
        for type_id, type_name in type_order:
            if type_id in resources_by_type:
                # Sort resources within each type alphabetically by name
                sorted_resources = sorted(resources_by_type[type_id], key=lambda x: x['Name'])
                sorted_types.append((type_name, sorted_resources))

        # Create sidebar page
        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='resources',
            title=localizedLabels.get('civilopediaCategoryResources'),
            pageLabel=localizedLabels.get('resourcesPageLabel'),
            helpText=localizedLabels.get('resourcesHelpText'),
            quoteText=localizedLabels.get('resourcesQuote'),
            grouped_items=sorted_types,
            labels=localizedLabels
        )
        with open(output_path / 'resources.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate terrain page with terrains and features separated
    if 'terrain' in data and len(data['terrain']) > 0:

        # Separate terrains and features
        from collections import defaultdict
        terrain_items = [item for item in data['terrain'] if not item.get('IsFeature', False)]
        feature_items = [item for item in data['terrain'] if item.get('IsFeature', False)]

        # Sort both groups alphabetically
        terrain_items_sorted = sorted(terrain_items, key=lambda x: x['Name'])
        feature_items_sorted = sorted(feature_items, key=lambda x: x['Name'])

        # Create grouped list
        grouped_terrain = []
        if terrain_items_sorted:
            grouped_terrain.append((localizedLabels.get('terrainsCategory'), terrain_items_sorted))
        if feature_items_sorted:
            grouped_terrain.append((localizedLabels.get('featuresCategory'), feature_items_sorted))

        # Debug output
        
        print(f"Generating terrain page with {len(terrain_items_sorted)} terrains and {len(feature_items_sorted)} features...")

        # Create sidebar page
        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='terrain',
            title=localizedLabels.get('civilopediaCategoryTerrains'),
            pageLabel=localizedLabels.get('terrainPageLabel'),
            helpText=localizedLabels.get('terrainHelpText'),
            quoteText=localizedLabels.get('terrainQuote'),
            grouped_items=grouped_terrain,
            labels=localizedLabels
        )
        with open(output_path / 'terrain.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate Great People / Specialists page
    # Generate great people page with specialists and great persons subsections
    if 'greatPeople' in data and isinstance(data['greatPeople'], dict):
        specialists = data['greatPeople'].get('specialists', [])
        great_persons = data['greatPeople'].get('greatPersons', [])

        print(f"Generating great people page with {len(specialists)} specialists and {len(great_persons)} great person units...")

        # Create two subsections with localized labels
        grouped_items = []

        if specialists:
            grouped_items.append((
                localizedLabels.get('specialistsLabel'),
                sorted(specialists, key=lambda x: x['Name'])
            ))

        if great_persons:
            grouped_items.append((
                localizedLabels.get('greatPersonsLabel'),
                sorted(great_persons, key=lambda x: x['Name'])
            ))

        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='greatPeople',
            title=localizedLabels.get('civilopediaCategorySpecialists'),
            pageLabel=localizedLabels.get('greatPeoplePageLabel'),
            helpText=localizedLabels.get('greatPeopleHelpText'),
            quoteText=localizedLabels.get('greatPeopleQuote'),
            grouped_items=grouped_items,
            labels=localizedLabels
        )
        with open(output_path / 'greatPeople.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate World Congress page with resolutions and league projects
    if ('resolutions' in data and len(data['resolutions']) > 0) or ('leagueProjects' in data and len(data['leagueProjects']) > 0):
        print(f"Generating World Congress page with {len(data.get('resolutions', []))} resolutions and {len(data.get('leagueProjects', []))} league projects...")

        grouped_congress = []

        # Add resolutions section
        if 'resolutions' in data and len(data['resolutions']) > 0:
            resolutions_label = localizedLabels.get('resolutionsCategory')
            sorted_resolutions = sorted(data['resolutions'], key=lambda x: x['Name'])
            grouped_congress.append((resolutions_label, sorted_resolutions))

        # Add league projects section
        if 'leagueProjects' in data and len(data['leagueProjects']) > 0:
            projects_label = localizedLabels.get('leagueProjectsCategory')
            sorted_projects = sorted(data['leagueProjects'], key=lambda x: x['Name'])
            grouped_congress.append((projects_label, sorted_projects))

        # Add league projects section
        if 'specialProjects' in data and len(data['specialProjects']) > 0:
            special_projects_label = localizedLabels.get('specialProjectsCategory')
            sorted_special_projects = sorted(data['specialProjects'], key=lambda x: x['Name'])
            grouped_congress.append((special_projects_label, sorted_special_projects))

        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='resolutions',
            title=localizedLabels.get('civilopediaCategoryWorldCongress'),
            pageLabel=localizedLabels.get('resolutionsPageLabel'),
            helpText=localizedLabels.get('worldCongressHelpText'),
            quoteText=localizedLabels.get('worldCongressQuote'),
            grouped_items=grouped_congress,
            labels=localizedLabels
        )
        with open(output_path / 'worldCongress.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate City-States page with trait grouping and sidebar
    if 'cityStates' in data and len(data['cityStates']) > 0:
        print(f"Generating city-states page with {len(data['cityStates'])} city-states...")

        # Group city-states by trait
        from collections import defaultdict
        citystates_by_trait = defaultdict(list)
        for citystate in data['cityStates']:
            trait_name = citystate.get('TraitName')
            trait_id = citystate.get('TraitID', 999)
            if trait_name:
                citystates_by_trait[(trait_id, trait_name)].append(citystate)

        # Sort by trait ID
        sorted_traits = sorted(citystates_by_trait.items(), key=lambda x: x[0][0])

        # Create grouped items with trait names as headers
        grouped_citystates = [(trait_name, sorted(citystates, key=lambda x: x['Name']))
                               for (trait_id, trait_name), citystates in sorted_traits]

        template = env.get_template('sidebar.html')
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page='cityStates',
            title=localizedLabels.get('civilopediaCategoryCityStates'),
            pageLabel=localizedLabels.get('cityStatesPageLabel'),
            helpText=localizedLabels.get('cityStatesHelpText'),
            quoteText=localizedLabels.get('cityStatesQuote'),
            grouped_items=grouped_citystates,
            labels=localizedLabels
        )
        with open(output_path / 'cityStates.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate pages for other categories with sidebar
    # Map category keys to (title, label_key)
    categories = [
        ('improvements', 'civilopediaCategoryImprovements', 'improvementsPageLabel'),
        ('corporations', 'civilopediaCategoryCorporations', 'corporationsPageLabel'),
    ]

    template = env.get_template('sidebar.html')

    for category_key, category_title, label_key in categories:
        # Skip if category doesn't exist in data
        if category_key not in data or len(data[category_key]) == 0:
            print(f"Skipping {localizedLabels.get(category_title)} (not in export data)")
            continue

        print(f"Generating {localizedLabels.get(category_title).lower()} page with {len(data[category_key])} entries...")

        # Single page with sidebar (no grouping for these categories)
        items = sorted(data[category_key], key=lambda x: x['Name'])
        html = template.render(
            version=CIVILOPEDIA_VERSION,
            civilopediaTitle=LOC['CIVILOPEDIA_TITLE'],
            searchPlaceholder=LOC['SEARCH_PLACEHOLDER'],
            noResultsFound=LOC['NO_RESULTS_FOUND'],
            active_page=category_key,
            title=localizedLabels.get(category_title),
            pageLabel=localizedLabels.get(label_key),
            helpText=localizedLabels.get(category_key+'HelpText'),
            quoteText=localizedLabels.get(category_key+'Quote'),
            grouped_items=[(None, items)],
            labels=localizedLabels
        )
        with open(output_path / f'{category_key}.html', 'w', encoding='utf-8') as f:
            f.write(html)

    # Generate global search index
    print("Generating global search index...")
    search_index = []

    # Helper function to add items to search index
    def add_to_search_index(items, page_key):
        for item in items:
            name = item.get('Name') or item.get('Description', '')
            item_type = item.get('Type', '')
            if name and item_type:
                # Apply Civ5 formatting conversion for display in search results
                formatted_name = convert_civ5_formatting(name)
                search_index.append({
                    'nameFormatted': formatted_name,  # HTML formatted for display
                    'name': name,  # Original for searching (includes color codes but that's ok)
                    'type': item_type,
                    'page': page_key
                })

    # Filter buildings to exclude wonders (they're in the wonders category)
    buildings_only = [b for b in data.get('buildings', [])
                      if not b.get('IsWonder') and not b.get('IsNationalWonder')]

    # Add all categories to search index
    categories_to_index = [
        ('concepts', data.get('concepts', [])),
        ('civilizations', data.get('civilizations', [])),
        ('civilizations', data.get('leaders', [])),
        ('units', data.get('units', [])),
        ('buildings', buildings_only),
        ('wonders', data.get('wonders', [])),
        ('technologies', data.get('technologies', [])),
        ('policies', data.get('policies', [])),
        ('beliefs', data.get('beliefs', [])),
        ('beliefs', data.get('religions', [])),
        ('promotions', data.get('promotions', [])),
        ('resources', data.get('resources', [])),
        ('improvements', data.get('improvements', [])),
        ('terrain', data.get('terrain', [])),
        ('cityStates', data.get('cityStates', [])),
        ('worldCongress', data.get('resolutions', [])),
        ('worldCongress', data.get('leagueProjects', [])),
        ('corporations', data.get('corporations', [])),
        ('greatPeople', data['greatPeople'].get('specialists', [])),
        ('greatPeople', data['greatPeople'].get('greatPersons', []))
    ]
    for page_key, items in categories_to_index:
        if items:
            add_to_search_index(items, page_key)

    # Write search index JavaScript file
    search_index_js = f"window.globalSearchIndex = {json.dumps(search_index, ensure_ascii=False, indent=2)};"
    with open(output_path / 'searchIndex.js', 'w', encoding='utf-8') as f:
        f.write(search_index_js)

    print(f"Generated search index with {len(search_index)} articles")

    print(f"\n{'='*50}")
    print(f"SUCCESS! Generated HTML files in: {output_path}")
    print(f"{'='*50}")


if __name__ == '__main__':

    abspath = os.path.abspath(__file__)
    dname = os.path.dirname(abspath)
    os.chdir(dname)
    
    json_file = JSON_INPUT_FILE_NAME

    if not os.path.exists(json_file):
        print(f"Error: File '{json_file}' not found. Run extract_from_log.py first")
        sys.exit(1)

    generate_html(json_file)
