<?php

/**
 * @file
 * Administrative page callbacks for the Before/After module.
 */

/**
 * Submit handler for adding a new option set.
 */
function beforeafter_form_optionset_add_submit($form, &$form_state) {
  $optionset = beforeafter_optionset_new(array('name' => $form_state['values']['name'], 'title' => $form_state['values']['title']));

  if (!($saved = beforeafter_optionset_save($optionset, TRUE))) {
    drupal_set_message(t('Failed to create option set. Please verify your settings.'), 'error');
    return;
  }

  drupal_set_message(t('Option set %name was created.', array('%name' => $optionset->name)));
  $form_state['redirect'] = 'admin/config/media/beforeafter/edit/' . $optionset->name;
}

/**
 * Form builder; Form to edit a given option set.
 */
function beforeafter_form_optionset_edit($form, &$form_state) {

  if (empty($form_state['optionset'])) {
    $optionset = beforeafter_optionset_new();
  } else {
    $optionset = $form_state['optionset'];
  }

  _beforeafter_typecast_optionset($optionset->options);

  $module_path = drupal_get_path('module', 'beforeafter');
  $easing_options = _beforeafter_easing_options();

  // Title
  $form['title'] = array(
    '#type' => 'textfield',
    '#maxlength' => '255',
    '#title' => t('Title'),
    '#description' => t('A human-readable title for this option set.'),
    '#required' => TRUE,
    '#default_value' => $optionset->title,
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => '255',
    '#machine_name' => array(
      'source' => array('title'),
      'exists' => 'beforeafter_optionset_exists',
    ),
    '#required' => TRUE,
    '#default_value' => $optionset->name,
  );

  if (module_exists('jqeasing')) {
    $form['info'] = array(
      '#type' => 'fieldset',
      '#title' => t('Info'),
      'easing' => array(
        '#markup' => '<p>heheheee</p>',
      ),
    );
  }

  $preview_id = drupal_html_id('beforeafter_preview');
  $js_settings = array_merge($optionset->options, array(
    'container' => $preview_id,
  ));

  $form['preview'] = array(
    '#type' => 'fieldset',
    '#title' => 'Preview',
    '#description' => t('Try your settings here. The options is applied to the preview in real-time.'),
    'buttons' => array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('form-item'),
      ),
      'reset' => array(
        '#prefix' => '<button' . drupal_attributes(array(
            'class' => array('beforeafter-preview--reset'),
          )) . '>',
        '#suffix' => '</button>',
        '#markup' => t('Reset'),
      ),
    ),
    'beforeafter' => array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('beforeafter', 'beforeafter-preview'),
        'id' => $preview_id,
      ),
      '#attached' => array(
        'library' => array(
          array('beforeafter', 'beforeafter'),
        ),
        'js' => array(
          $module_path . '/js/beforeafter.preview.js',
          $module_path . '/js/beforeafter.admin.js',
          array(
            'data' => array('beforeafterPreview' => $js_settings),
            'type' => 'setting',
          ),
        ),
      ),
      'after' => array(
        '#markup' => '<img' . drupal_attributes(array(
            'src' => url($module_path . '/demo/preview_after.jpg'),
            'alt' => '',
          )) . '/>',
      ),
      'before' => array(
        '#markup' => '<img' . drupal_attributes(array(
            'src' => url($module_path . '/demo/preview_before.jpg'),
            'alt' => '',
          )) . '/>',
      ),
    ),
  );

  // Options Vertical Tab Group table
  $form['options'] = array(
    '#type' => 'vertical_tabs',
  );
  $form['options']['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );
  $form['options']['intro'] = array(
    '#type' => 'fieldset',
    '#title' => t('Intro'),
  );
  $form['options']['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced options'),
  );
  $integer_fields = array();

  $general =& $form['options']['general'];
  if (!module_exists('jqeasing')) {
    $general['info_easing'] = array(
      '#type' => 'item',
      '#title' => t('Tip'),
      '#markup' =>
        '<div class="description">' .
        t('To get more animation easing functions you can install the <a href="!url" target="_blank">jQuery Easing</a> module.', array(
          '!url' => url('http://drupal.org/project/jqeasing', array('absolute' => TRUE)),
        )) .
        '</div>',
    );
  }
  $general['theme'] = array(
    '#type' => 'select',
    '#title' => t('Theme'),
    '#options' => _beforeafter_theme_options(),
    '#default_value' => $optionset->options['theme'],
  );
  $general['init_position'] = array(
    '#type' => 'textfield',
    '#title' => t('Initial position'),
    '#description' => t('The position of the drag handle on initialization. Accepts values between 0 (left) and 1 (right).'),
    '#attributes' => array('style' => 'width: 60px'),
    '#size' => 4,
    '#default_value' => $optionset->options['init_position'],
  );
  $general['show_links'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show full links'),
    '#description' => t('Whether or not to display links below the image that a visitor can click on that will automatically show the full before or full after image.'),
    '#default_value' => $optionset->options['show_links'],
  );
  $general['show_links_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Links'),
    '#states' => array(
      'visible' => array(
        ':input[name="show_links"]' => array('checked' => TRUE),
      ),
    ),
  );
  $links_options =& $general['show_links_options'];

  $links_options['before_link_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Before link text'),
    '#description' => t('This is the text used for the before image link.'),
    '#default_value' => $optionset->options['before_link_text'],
  );
  $links_options['after_link_text'] = array(
    '#type' => 'textfield',
    '#title' => t('After link text'),
    '#description' => t('This is the text used for the after image link.'),
    '#default_value' => $optionset->options['after_link_text'],
  );

  $intro =& $form['options']['intro'];
  $intro['intro_animate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Animate'),
    '#description' => t('Whether or not to animate the drag bar to a designated position on page load.'),
    '#default_value' => $optionset->options['intro_animate'],
  );
  $intro['intro_animate_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Animate intro'),
    '#states' => array(
      'visible' => array(
        ':input[name="intro_animate"]' => array('checked' => TRUE),
      ),
    ),
  );
  $intro_anim =& $intro['intro_animate_options'];
  $intro_anim['intro_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Delay'),
    '#description' => t('The number of milliseconds to wait before starting the animation.'),
    '#field_suffix' => 'ms',
    '#size' => 4,
    '#attributes' => array('style' => 'width: 60px'),
    '#default_value' => $optionset->options['intro_delay'],
  );
  $integer_fields[] =& $intro_anim['intro_delay'];

  $intro_anim['intro_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Duration'),
    '#description' => t('The duration of the animation in milliseconds.'),
    '#field_suffix' => 'ms',
    '#size' => 4,
    '#attributes' => array('style' => 'width: 60px'),
    '#default_value' => $optionset->options['intro_duration'],
  );
  $integer_fields[] =& $intro_anim['intro_duration'];

  $intro_anim['intro_easing'] = array(
    '#type' => 'select',
    '#title' => t('Easing'),
    '#description' => t('The timing function used for the animation.'),
    '#options' => $easing_options,
    '#default_value' => $optionset->options['intro_easing'],
  );


  $advanced =& $form['options']['advanced'];
  $advanced['click_anim'] = array(
    '#type' => 'fieldset',
    '#title' => t('Click animation'),
    '#description' => t('Animation settings used to move the drag bar when the user clicks somewhere in the container.'),
  );
  $advanced['click_anim']['click_anim_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Delay'),
    '#description' => t('The number of milliseconds to wait before starting the animation.'),
    '#field_suffix' => 'ms',
    '#size' => 4,
    '#attributes' => array('style' => 'width: 60px'),
    '#default_value' => $optionset->options['click_anim_delay'],
  );
  $integer_fields[] =& $advanced['click_anim']['click_anim_delay'];

  $advanced['click_anim']['click_anim_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Duration'),
    '#description' => t('The duration of the animation in milliseconds.'),
    '#field_suffix' => 'ms',
    '#size' => 4,
    '#attributes' => array('style' => 'width: 60px'),
    '#default_value' => $optionset->options['click_anim_duration'],
  );
  $integer_fields[] =& $advanced['click_anim']['click_anim_duration'];

  $advanced['click_anim']['click_anim_easing'] = array(
    '#type' => 'select',
    '#title' => t('Easing'),
    '#description' => t('The timing function used for the animation.'),
    '#options' => $easing_options,
    '#default_value' => $optionset->options['click_anim_easing'],
  );
  $advanced['link_anim'] = array(
    '#type' => 'fieldset',
    '#title' => t('Link animation'),
    '#description' => t('Animation settings used to move the drag bar when the user clicks the Before or After links, if enabled.'),
  );
  $advanced['link_anim']['link_anim_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Delay'),
    '#description' => t('The number of milliseconds to wait before starting the animation.'),
    '#field_suffix' => 'ms',
    '#size' => 4,
    '#attributes' => array('style' => 'width: 60px'),
    '#default_value' => $optionset->options['link_anim_delay'],
  );
  $integer_fields[] =& $advanced['link_anim']['link_anim_delay'];

  $advanced['link_anim']['link_anim_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Duration'),
    '#description' => t('The duration of the animation in milliseconds.'),
    '#field_suffix' => 'ms',
    '#size' => 4,
    '#attributes' => array('style' => 'width: 60px'),
    '#default_value' => $optionset->options['link_anim_duration'],
  );
  $integer_fields[] =& $advanced['link_anim']['link_anim_duration'];

  $advanced['link_anim']['link_anim_easing'] = array(
    '#type' => 'select',
    '#title' => t('Easing'),
    '#description' => t('The timing function used for the animation.'),
    '#options' => $easing_options,
    '#default_value' => $optionset->options['link_anim_easing'],
  );

  if (module_exists('flexslider')) {
    $form['options']['flexslider'] = array(
      '#type' => 'fieldset',
      '#title' => t('FlexSlider integration'),
    );
    $flexslider =& $form['options']['flexslider'];
    $flexslider['flexslider_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable FlexSlider integration'),
      '#description' => t('Makes the widget aware if it\'s placed inside a FlexSlider slide.'),
      '#default_value' => $optionset->options['flexslider_enable'],
    );
    $flexslider['flexslider_options'] = array(
      '#type' => 'fieldset',
      '#title' => t('FlexSlider integration settings'),
      '#states' => array(
        'visible' => array(
          ':input[name="flexslider_enable"]' => array('checked' => TRUE),
        ),
      ),
    );
    $flexslider_options =& $flexslider['flexslider_options'];
    $flexslider_options['flexslider_selector'] = array(
      '#type' => 'textfield',
      '#title' => t('Selector'),
      '#field_prefix' => '$(this).closest(',
      '#field_suffix' => ')',
      '#attributes' => array('style' => 'width: 100px'),
      '#description' => t('The selector used to target the flexslider element.'),
      '#default_value' => $optionset->options['flexslider_selector'],
    );
    $flexslider_options['flexslider_anim_callback'] = array(
      '#type' => 'select',
      '#title' => t('Animate callback'),
      '#description' => t('The FlexSlider callback trigger animation, similar to the intro animation.'),
      '#options' => array(
        '' => t('None (disable)'),
        'after' => 'after',
        'before' => 'before',
      ),
      '#default_value' => $optionset->options['flexslider_anim_callback'],
    );
    $flexslider_options['flexslider_anim_delay'] = array(
      '#type' => 'textfield',
      '#title' => t('Delay'),
      '#description' => t('The number of milliseconds to wait before starting the animation.'),
      '#field_suffix' => 'ms',
      '#size' => 4,
      '#attributes' => array('style' => 'width: 60px'),
      '#default_value' => $optionset->options['flexslider_anim_delay'],
    );
    $integer_fields[] =& $flexslider_options['flexslider_anim_delay'];

    $flexslider_options['flexslider_anim_duration'] = array(
      '#type' => 'textfield',
      '#title' => t('Duration'),
      '#description' => t('The duration of the animation in milliseconds.'),
      '#field_suffix' => 'ms',
      '#size' => 4,
      '#attributes' => array('style' => 'width: 60px'),
      '#default_value' => $optionset->options['flexslider_anim_duration'],
    );
    $integer_fields[] =& $flexslider_options['flexslider_anim_duration'];

    $flexslider_options['flexslider_anim_easing'] = array(
      '#type' => 'select',
      '#title' => t('Easing'),
      '#description' => t('The timing function used for the animation.'),
      '#options' => $easing_options,
      '#default_value' => $optionset->options['flexslider_anim_easing'],
    );
  }


  if (module_exists('elements')) {
    foreach ($integer_fields as &$field) {
      $field['#type'] = 'numberfield';
      $field['#min'] = 0;
      $field['#step'] = 1;
      $field['#number_type'] = 'integer';
    }
  }

  return $form;
}

/**
 * Form builder; Form to delete a given option set.
 */
function beforeafter_optionset_form_delete($form, &$form_state, $optionset) {
  $form_state['optionset'] = &$optionset;

  // Deleting an export in code will revert it.
  $op = ($optionset->export_type & EXPORT_IN_CODE) ? 'Revert' : 'Delete';

  return confirm_form(
    $form,
    t('Are you sure you want to @action the option set %name?', array('@action' => t(drupal_strtolower($op)), '%name' => $optionset->name)),
    'admin/config/media/beforeafter',
    NULL,
    t($op),  t('Cancel')
  );
}

/**
 * Submit handler for deleting an option set.
 */
function beforeafter_optionset_form_delete_submit($form, &$form_state) {
  $optionset = &$form_state['optionset'];
  $op = ($optionset->export_type & EXPORT_IN_CODE) ? 'reverted' : 'deleted';

  ctools_include('export');
  ctools_export_crud_delete('beforeafter_optionset', $optionset);

  drupal_set_message(t('Option set %name was ' . $op . '.', array('%name' => $optionset->name)));
  $form_state['redirect'] = 'admin/config/media/beforeafter';
}