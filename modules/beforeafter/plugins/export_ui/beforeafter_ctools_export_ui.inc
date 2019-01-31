<?php

/**
 * @file
 * CTools export UI plugin
 */


/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'schema' => 'beforeafter_optionset',  // As defined in hook_schema().
  'access' => 'administer beforeafter',  // Define a permission users must have to access these pages.

  // Define the menu item.
  'menu' => array(
    'menu prefix' => 'admin/config/media',
    'menu item' => 'beforeafter',
    'menu title' => 'Before/After',
    'menu description' => 'Administer Before/After presets',
  ),

  // Define user interface texts.
  'title singular' => t('optionset'),
  'title plural' => t('optionsets'),
  'title singular proper' => t('Before/After option set'),
  'title plural proper' => t('Before/After option sets'),

  // Define the names of the functions that provide the add/edit forms.
  'form' => array(
    'settings' => 'beforeafter_ctools_export_ui_form',
    'validate' => 'beforeafter_ctools_export_ui_form_validate',
    'submit' => 'beforeafter_ctools_export_ui_form_submit',
  ),
);

/**
 * Export UI form
 */
function beforeafter_ctools_export_ui_form(&$form, &$form_state) {
  // Load the admin form include
  module_load_include('inc', 'beforeafter', 'beforeafter.admin');

  // Make optionset reference in form_state
  $form_state['optionset'] = &$form_state['item'];

  // Load the configuration form
  $form = drupal_retrieve_form('beforeafter_form_optionset_edit', $form_state);
}

/**
 * Validation handler
 */
function beforeafter_ctools_export_ui_form_validate(&$form, &$form_state) {
}

/**
 * Submit handler
 */
function beforeafter_ctools_export_ui_form_submit(&$form, &$form_state) {
  // Edit the reference to $form_state['optionset'] which will in turn
  // reference $form_state['item'] which is what CTools is looking for.
  $optionset = &$form_state['optionset'];
  $optionset->title = $form_state['values']['title'];

  // Assign the values to the option set
  $optionset->options = _beforeafter_optionset_defaults();

  // Save all the values for the optionset
  foreach ($optionset->options as $key => $value) {
    if (array_key_exists($key, $form_state['values'])) {
      $optionset->options[$key] = $form_state['values'][$key];
    }
  }
}