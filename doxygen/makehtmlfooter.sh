#!/bin/sh

# Generates the HTML footer

echo '<html>'
echo '</body>'
echo '<div id="footer" style="background-color:#DDDDDD;">'
echo '<small>'
# echo '<img class="footer" src="ACTARSIM_logo_small.png" alt="ACTARSim"/></a>'
# Doxygen unconditionally adds a space in front of $DOXYGEN_ROOT_VERSION
echo ''ACTARSIM' - Reference Guide Generated on $datetime using Doxygen.'
echo '</small>'
echo '</div>'
echo '</body>'
echo '</html>'
