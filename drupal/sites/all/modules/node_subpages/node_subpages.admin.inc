<?php
/**
 * @file
 * Admin forms and functionality for Node Subpages module
 */

/**
 * List the subpages for a node type
 */
function node_subpages_admin_list($form, &$form_state, $type) {
  $type_url_str = str_replace('_', '-', $type->type);
  
  $subpages = node_subpages_list_for_type($type->type);

  $form['#tree'] = TRUE;
  $form['#theme'] = 'node_subpages_admin_list';
  
  foreach ($subpages as $path => $details) {
    if ($details['default_view_page']) {
      $tab_type = 'Default tab';
    }
    elseif ($details['field']) {
      $tab_type = 'Field: ' . $details['field'];
    }
    elseif ($details['view']) {
      $tab_type = 'View: ' . $details['view'];
    }
    else {
      $tab_type = 'Unknown';
    }
    
    $form['subpages'][$path]['id'] = array(
      '#type' => 'value',
      '#value' => $details['id'],
    );
    $form['subpages'][$path]['weight'] = array(
      '#type' => 'weight', 
      '#title' => t('Weight for @title', array('@title' => $details['title'])),
      '#title_display' => 'invisible', 
      '#default_value' => $details['weight'], 
      '#delta' => 10,
    );
    $form['subpages'][$path]['subpath'] = array(
      '#markup' => $path,
    );
    $form['subpages'][$path]['title'] = array(
      '#markup' => $details['title'],
    );
    $form['subpages'][$path]['type'] = array(
      '#markup' => $tab_type,
    );

    if ($details['id'] > 0) {
      $form['subpages'][$path]['edit'] = array(
        '#markup' => l('edit', 'admin/structure/types/manage/' . $type_url_str . '/subpages/edit/' . $details['id'])
      );
      $form['subpages'][$path]['delete'] = array(
        '#markup' => l('delete', 'admin/structure/types/manage/' . $type_url_str . '/subpages/delete/' . $details['id']),
      );
    }
  }
  
  $form['add_another'] = array(
    '#markup' => l('+ Add another subpage', 'admin/structure/types/manage/' . $type_url_str . '/subpages/add'),
  );
  
  if (!empty($form['subpages'])) {
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save changes'));
  }
    
  return $form;
}


/**
 * Save the new subpage order
 */
function node_subpages_admin_list_submit($form, &$form_state) {
  $saved = 0;
  foreach ($form_state['values']['subpages'] as $path => $values) {
    $record = array('id' => $values['id'], 'weight' => $values['weight']);
    drupal_write_record('node_subpages', $record, 'id');
    $saved++;
  }
  if ($saved > 0) {
    drupal_set_message(t('Subpage order saved.'));
  }
  else {
    drupal_set_message(t('No subpages to save.'));
  }
}


/**
 * Theme the admin subpages list
 */
function theme_node_subpages_admin_list($vars) {
  $form = $vars['form'];
  
  $rows = array();  
  if (!empty($form['subpages'])) {
    foreach (element_children($form['subpages']) as $path) {
      $form['subpages'][$path]['weight']['#attributes']['class'] = array('subpage-weight');
      $rows[] = array(
        'data' => array(
          drupal_render($form['subpages'][$path]['subpath']),
          drupal_render($form['subpages'][$path]['title']),
          drupal_render($form['subpages'][$path]['type']),
          drupal_render($form['subpages'][$path]['edit']),
          drupal_render($form['subpages'][$path]['delete']),
          drupal_render($form['subpages'][$path]['weight']),
        ),
        'class' => array('draggable'),
      );
    }
  }
  
  $headers = array(
    t('Sub-path'),
    t('Title'),
    t('Type'),
    array('data' => t('Operations'), 'colspan' => 2),
  );

  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('No subpages have been added for this content type.'),
        'colspan' => count($headers),
      ),
    );
  }
  else {
    $headers[] = t('Weight');
    
    // Make the table drag-sortable
    drupal_add_tabledrag('admin_subpage_list', 'order', 'sibling', 'subpage-weight');
  }
  
  $table_vars = array(
    'header' => $headers, 
    'rows' => $rows, 
    'attributes' => array('id' => 'admin_subpage_list'),
  );

  // Rend the Add Another link
  $output = drupal_render($form['add_another']);
  
  // Render the table listing the subpages
  $output .= theme('table', $table_vars);
  
  // Render the submit button
  $output .= drupal_render($form['actions']);
  
  // Render the last hidden elements and such
  $output .= drupal_render_children($form);
  return $output;
}




/**
 * Form to add a new subpage to the given content type
 */
function node_subpages_add_subpage($form, &$form_state, $type, $id = NULL) {
  $form = array();
  $defaults = array(
    'subpath' => '',
    'title' => '',
    'field' => '',
    'view' => '',
    'view_display' => '',
  );

  // If editing an existing subpage, load the details for use as default
  // values in the form
  if ($id > 0) {
    $form['#id'] = $id;
    $defaults = node_subpages_load_specific($id);
  }


  $form['#node_type'] = $type;

  $form['subpath'] = array(
    '#type' => 'textfield',
    '#title' => t('Subpath'),
    '#description' => t('Subpath that will be used in the URL for this subpage on the node.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $defaults['subpath'],
  );
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Tab Title'),
    '#description' => t('Displayed as the text of the tab link for this subpage.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $defaults['title'],
  );

  $form['data_source'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data Source'),
    '#description' => t('Select the source for the content of this subpage. Only choose one.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );


  if (module_exists('field')) {
    $fields = field_info_instances('node', $type->type);
    if ($fields) {
      $options = array('' => '-- Select One --');
      foreach ($fields as $field_key => $field_details) {
        // Only allow Text fields to be used for subpages
        if ($field_details['widget']['module'] == 'text') {
          $options[$field_key] = $field_details['label'] . ' (' . $field_key . ')';
        }
      }

      $form['data_source']['field'] = array(
        '#type' => 'select',
        '#title' => t('CCK Field'),
        '#multiple' => FALSE,
        '#description' => t('CCK field to use as the content of this subpage.'),
        '#options' => $options,
        '#default_value' => $defaults['field'],
      );
    }
  }

  if (module_exists('views')) {
    $views = views_get_all_views();
    $options = array('' => '-- Select One --');
    foreach ($views as $view_name => $view) {
      foreach ($view->display as $display_name => $display) {
        $options[$view_name . ':' . $display_name] = $view_name . ': ' . $display->display_title;
      }
    }

    $form['data_source']['view'] = array(
      '#type' => 'select',
      '#title' => t('View'),
      '#multiple' => FALSE,
      '#description' => t('View to display as content of the subpage.'),
      '#options' => $options,
      '#default_value' => $defaults['view'] . ':' . $defaults['view_display'],
    );
  }

  if ($id > 0) {
    $form['update'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
    );
  }
  else {
    $form['add'] = array(
      '#type' => 'submit',
      '#value' => t('Add'),
    );
  }
  return $form;


}


/**
 * Submit the subpage add form, to create a new subpage type
 */
function node_subpages_add_subpage_submit($form, &$form_state) {
  $values = $form_state['values'];
  $node_type = $form['#node_type'];

  $record = array(
    'node_type' => $node_type->type,
    'subpath' => $values['subpath'],
    'title' => $values['title'],
  );

  if ($values['field']) {
    $record['source_type'] = 'cck';
    $record['source_data'] = $values['field'];
  }
  elseif ($values['view']) {
    $record['source_type'] = 'view';
    $record['source_data'] = $values['view'];
  }

  if ($form['#id'] > 0) {
    $record['id'] = $form['#id'];
    $saved = drupal_write_record('node_subpages', $record, 'id');
  }
  else {
    $saved = drupal_write_record('node_subpages', $record);
  }

  if ($saved) {
    if ($saved == SAVED_NEW) {
      drupal_set_message(t('New subpage saved.'));
    }
    else {
      drupal_set_message(t('Subpage updated.'));
    }

    $type_url_str = str_replace('_', '-', $node_type->type);

    // Rebuild the menu cache so that the path is updated
    menu_rebuild();

    drupal_goto('admin/structure/types/manage/' . $type_url_str . '/subpages');
  }
  else {
    drupal_set_message(t('Error saving subpage. Please try again.'));
    return FALSE;
  }
}


/**
 * Form to delete subpage: confirm before deleting
 */
function node_subpages_delete_subpage($form, &$form_state, $node_type, $id) {
  $type_url_str = str_replace('_', '-', $node_type->type);
  $cancel_path = 'admin/structure/types/manage/' . $type_url_str . '/subpages';
  $details = node_subpages_load_specific($id);
  $question = t('Are you sure you want to delete the %subpage subpage from %content_type?', array(
    '%subpage' => $details['title'],
    '%content_type' => $node_type->name,
  ));

  $form = array();
  $form['#id'] = $id;
  $form['#node_type'] = $node_type;
  return confirm_form($form, $question, $cancel_path, NULL, t('Delete'));
}

/**
 * Submit the subpage deletion confirmation form
 */
function node_subpages_delete_subpage_submit($form, &$form_state) {
  db_delete('node_subpages')
  ->condition('id', $form['#id'])
  ->execute();
  drupal_set_message(t('Subpage deleted.'));

  $type_url_str = str_replace('_', '-', $form['#node_type']->type);
  $redirect_path = 'admin/structure/types/manage/' . $type_url_str . '/subpages';
  drupal_goto($redirect_path);
}
