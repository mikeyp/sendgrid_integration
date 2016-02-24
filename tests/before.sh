#!/bin/bash


#
# Ensures that the module is linked into the Drupal code base.
#
function drupal_ti_send_ensure_module_linked() {
	# Ensure we are in the right directory.
	cd "$DRUPAL_TI_DRUPAL_DIR"

	# This function is re-entrant.
	if [ -L "$DRUPAL_TI_MODULES_PATH/$DRUPAL_TI_MODULE_NAME" ]
	then
		return
	fi

	# Find absolute path to module.
	MODULE_DIR=$(cd "$TRAVIS_BUILD_DIR"; pwd)

	# Ensure directory exists.

	mkdir -p "$DRUPAL_TI_MODULES_PATH"

	# Point module into the drupal installation.
	ln -sf "$MODULE_DIR" "$DRUPAL_TI_MODULES_PATH/$DRUPAL_TI_MODULE_NAME"
}


#
# Ensures that the module is linked into the Drupal code base
# and enabled.
#
function drupal_ti_send_ensure_module() {
	# Ensure the module is linked into the code base.
	drupal_ti_send_ensure_module_linked

	# Enable it to download dependencies.
	drush --yes en "$DRUPAL_TI_MODULE_NAME"
	drush cc drush
}




# Simple script to install drupal for travis-ci running.

set -e $DRUPAL_TI_DEBUG

# Ensure the right Drupal version is installed.
echo "Ensure the right Drupal Version."
drupal_ti_ensure_drupal

# Enable simpletest module.
cd "$DRUPAL_TI_DRUPAL_DIR"
echo "DRUPAL TI - Drush Enable Simpletest module"
drush --yes en simpletest
echo "DRUPAL TI - Download Composer module and enable"
drush dl composer-8.x-1.x
drush en -y composer
echo "DRUPAL TI - Delete cache dir"
rm -f "$DRUPAL_TI_CACHE_DIR"/HOME/.drush/cache


# Ensure the module is linked into the code base and enabled.
echo "DRUPAL TI - Ensure the module is linked into the code base and enabled"
drupal_ti_send_ensure_module

# Clear caches and run a web server.
echo "DRUPAL TI - Clear caches"
drupal_ti_clear_caches
echo "DRUPAL TI - Run a web server"
drupal_ti_run_server

