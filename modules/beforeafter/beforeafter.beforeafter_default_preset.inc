<?php

/**
 * @file
 *
 */

/**
 * Implements hook_beforeafter_default_presets().
 */
function beforeafter_beforeafter_default_presets() {
  $presets = array();

  $preset = beforeafter_optionset_new();
  $preset->disabled = FALSE; /* Edit this to true to make a default preset disabled initially */
  $preset->api_version = 1;
  $preset->name = 'default';
  $preset->title = 'Default';
  $presets['default'] = $preset;

  return $presets;
}
