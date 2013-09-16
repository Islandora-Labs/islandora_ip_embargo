<?php
/**
 * @file
 * This file holds forms for islandora_ip_embargo
 */

/**
 * Configure IP Embargo Settings.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_ip_embargo_settings_form($form, &$form_state) {
  $form['islandora_ip_embargo_embargoed_redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Object embargoed redirect'),
    '#description' => t('The URL to go to when an object is embaroged.'),
    '#default_value' => variable_get(
      'islandora_ip_embargo_embargoed_redirect',
      ''
    ),
  );
  $form['islandora_ip_embargo_days_before_embargo_trigger'] = array(
    '#type' => 'textfield',
    '#title' => t('Days before embargo alert'),
    '#description' => t('How many days before an embargo is lifted to trigger rules.'),
    '#default_value' => variable_get(
      'islandora_ip_embargo_days_before_embargo_trigger',
      0
    ),
  );

  return system_settings_form($form);
}

/**
 * Populate data for the embargo object list form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_ip_embargo_manage_embargo_list_form($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');

  $options = array();
  $embargoes = islandora_ip_embargo_get_all_embargo_objects();

  // Loop thru all the pids, populate the objects and get
  // relationships.
  while ($data = $embargoes->fetchObject()) {
    $fedora_object = islandora_object_load($data->pid);
    $rels = $fedora_object->relationships->get();

    $options[$data->pid]['modify'] = l($fedora_object->label, 'islandora/object/' . $data->pid . '/ip_embargo');
    $options[$data->pid]['pid'] = l($data->pid, 'islandora/object/' . $data->pid . '/manage');
    $options[$data->pid]['collection'] = l($rels[1]['object']['value'], 'islandora/object/' . $rels[0]['object']['value']);
  }

  $header = array(
    'modify' => t('Modify'),
    'pid' => t('Manage'),
    'collection' => t('Is Member Of'),
  );

  $form['Lists'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#attributes' => array(),
    '#empty' => t("There are no embargoed objects."),
  );
  $form['pager'] = array(
    '#markup' => theme('pager'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#submit' => array('islandora_ip_embargo_delete_object_embargo_form_name_submit'),
  );

  return $form;
}

/**
 * Delete embargo's on affected object(s).
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_delete_object_embargo_form_name_submit($form, &$form_state) {
  foreach ($form_state['values']['Lists'] as $list_identifier => $is_checked) {
    if ($is_checked) {
      islandora_ip_embargo_remove_embargo($list_identifier);
    }
  }
}

/**
 * Configure IP Embargo lists.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_ip_embargo_manage_lists_form($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');

  $options = array();
  $results = islandora_ip_embargo_get_lists();
  while ($data = $results->fetchObject()) {
    $options[$data->lid]['name'] = l($data->name, 'admin/islandora/ip_embargo/lists/' . $data->lid);
  }
  $header = array('name' => t('Name'));
  $form['Lists'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#attributes' => array(),
    '#empty' => t("There are no lists."),
  );
  $form['pager'] = array(
    '#markup' => theme('pager'),
  );

  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  $form['Add'] = array(
    '#markup' => l(t('Add list'), 'admin/islandora/ip_embargo/lists/add'),
  );

  return $form;
}

/**
 * Will set error if the lists to be deleted have embargoes or ranges.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_manage_lists_form_validate($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  foreach ($form_state['values']['Lists'] as $list_identifier => $is_checked) {
    if ($is_checked) {
      $embargoes_results = islandora_ip_embargo_get_embargos($list_identifier);
      if ($embargoes_results->rowCount()) {
        form_set_error('list', t("You can't delete a list with embargoes."));
      }
      $ranges_results = islandora_ip_embargo_get_lists_information($list_identifier);
      if ($ranges_results->rowCount()) {
        form_set_error('list', t("You can't delete a list with ranges."));
      }
    }
  }
}

/**
 * Configure IP Embargo lists submit handler.
 *
 * This will delete lists.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_manage_lists_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  foreach ($form_state['values']['Lists'] as $list_identifier => $is_checked) {
    if ($is_checked) {
      islandora_ip_embargo_remove_list($list_identifier);
    }
  }
}

/**
 * Add an IP Embargo list.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_ip_embargo_add_list_form($form, &$form_state) {
  $form['name'] = array(
    '#title' => t('Name'),
    '#description' => t('Name of the list.'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );
  $form['submit'] = array('#type' => 'submit', '#value' => t('Create list'));

  return $form;
}

/**
 * Add an IP Embargo list.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_add_list_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  islandora_ip_embargo_add_list($form_state['values']['name']);
  $form_state['redirect'] = 'admin/islandora/ip_embargo';
}

/**
 * Configure an IP Embargo list's name and ranges.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param int $list_identifier
 *   The identifier of the list.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_ip_embargo_edit_list_form($form, &$form_state, $list_identifier) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  $list_information = islandora_ip_embargo_get_lists_information($list_identifier);
  $list_name = islandora_ip_embargo_get_list_name($list_identifier);
  $embargo_information = islandora_ip_embargo_get_embargos($list_identifier);
  $form_state['storage']['islandora_ip_embargo']['list_identifier'] = $list_identifier;
  $range_header = array('low_end' => t('low end'), 'high_end' => t('high end'));
  $range_options = array();
  while ($data = $list_information->fetchObject()) {
    $range_options[$data->rid] = array(
      'high_end' => $data->high_end,
      'low_end' => $data->low_end,
    );
  }
  $embargo_header = array(
    'embargoed_object' => t('embargoed object'),
    'expiry_date' => t('expiry date'),
    'object_pid' => t('pid'),
  );
  $embargo_options = array();
  while ($data = $embargo_information->fetchObject()) {
    $object = islandora_object_load($data->pid);
    if (isset($data->expiry)) {
      $year = date('Y', $data->expiry);
      $month = date('M', $data->expiry);
      $day = date('j', $data->expiry);
      $expiry_date = $day . '-' . $month . '-' . $year;
    }
    else {
      $expiry_date = t('None');
    }
    $embargo_options[$data->pid] = array(
      'embargoed_object' => l($object->label, "islandora/object/{$data->pid}/ip_embargo"),
      'expiry_date' => $expiry_date,
      'object_pid' => $data->pid,
    );
  }

  $form['name'] = array(
    '#title' => t('List Name'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $list_name,
  );
  $form['name_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update name'),
    '#submit' => array('islandora_ip_embargo_edit_list_form_name_submit'),
  );
  $form['list_tabs'] = array(
    '#type' => 'vertical_tabs',
  );
  $form['list_tabs']['list_ranges'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ranges'),
  );
  $form['list_tabs']['list_ranges']['ranges'] = array(
    '#type' => 'tableselect',
    '#header' => $range_header,
    '#options' => $range_options,
    '#attributes' => array(),
    '#empty' => t("There are no ranges."),
  );
  $form['list_tabs']['list_ranges']['pager'] = array(
    '#markup' => theme('pager'),
  );
  $form['list_tabs']['list_ranges']['delete_range'] = array(
    '#type' => 'submit',
    '#value' => t('Delete ranges'),
    '#submit' => array('islandora_ip_embargo_edit_list_form_range_submit'),
  );
  $form['list_tabs']['list_ranges']['Add'] = array(
    '#markup' => l(t('Add range'), "admin/islandora/ip_embargo/ranges/add/$list_identifier"),
  );
  $form['list_tabs']['list_embargoes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Embargoes'),
    '#access' => user_access('control islandora ip embargoes'),
  );
  $form['list_tabs']['list_embargoes']['embargoes'] = array(
    '#type' => 'tableselect',
    '#header' => $embargo_header,
    '#options' => $embargo_options,
    '#attributes' => array(),
    '#empty' => t("There are no embargoes"),
  );
  $form['list_tabs']['list_embargoes']['embargo_pager'] = array(
    '#markup' => theme('pager'),
  );
  $form['list_tabs']['list_embargoes']['delete_embargo'] = array(
    '#type' => 'submit',
    '#value' => t('Delete embargoes'),
    '#submit' => array('islandora_ip_embargo_edit_list_form_embargo_submit'),
  );
  return $form;
}

/**
 * Update the name of a list.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_edit_list_form_name_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  islandora_ip_embargo_edit_list(
    $form_state['values']['name'],
    $form_state['storage']['islandora_ip_embargo']['list_identifier']);
}

/**
 * Delete an IP address range on a list.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_edit_list_form_range_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  foreach ($form_state['values']['ranges'] as $range_identifier => $is_checked) {
    if ($is_checked) {
      islandora_ip_embargo_remove_range($range_identifier);
    }
  }
}

/**
 * Delete an embargo from a list.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_edit_list_form_embargo_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  foreach ($form_state['values']['embargoes'] as $embargo_identifier => $is_checked) {
    if ($is_checked) {
      islandora_ip_embargo_remove_embargo($embargo_identifier);
    }
  }
}

/**
 * Create an IP address range on a list.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param int $list_identifier
 *   The identifier of the list.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_ip_embargo_add_range_form($form, &$form_state, $list_identifier) {
  $form['low_end'] = array(
    '#title' => t('Lowend'),
    '#description' => t('low end of the IP range.'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );
  $form['high_end'] = array(
    '#title' => t('Highend'),
    '#description' => t('High end of the IP range.'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );
  $form_state['storage']['islandora_ip_embargo']['list_identifier'] = $list_identifier;
  $form['submit'] = array('#type' => 'submit', '#value' => t('Create range'));
  return $form;
}

/**
 * Submit handler for adding an IP range.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_add_range_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  $list_identifier = $form_state['storage']['islandora_ip_embargo']['list_identifier'];

  islandora_ip_embargo_add_ip_range(
    $list_identifier,
    $form_state['values']['low_end'],
    $form_state['values']['high_end']
  );

  $form_state['redirect'] = "admin/islandora/ip_embargo/lists/$list_identifier";
}

/**
 * Configure an IP range embargo on the object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_ip_embargo_object_embargo_form($form, &$form_state, $islandora_object) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  $form_state['storage']['islandora_ip_embargo']['update'] = FALSE;
  $form_state['storage']['islandora_ip_embargo']['pid'] = $islandora_object->id;
  $list_results = islandora_ip_embargo_get_lists();
  $options = array();
  while ($data = $list_results->fetchObject()) {
    $options[$data->lid] = $data->name;
  }

  if (empty($options)) {
    $form['empty'] = array(
      '#markup' => '<p>There are currently no IP address range lists defined. At least one IP address range list must be defined in order to create IP embargoes.</p>',
    );
  }
  else {
    $embargo_result = islandora_ip_embargo_get_embargo($islandora_object->id);
    $embargo_info = $embargo_result->fetchObject();
    if ($embargo_result->rowCount()) {
      if (user_access('administer islandora ip embargoes')) {
        $list = l($options[$embargo_info->lid], 'admin/islandora/ip_embargo/lists/' . $embargo_info->lid);
      }
      else {
        $list = $options[$embargo_info->lid];
      }
      if (!isset($embargo_info->expiry)) {
        $embargo_message = t('This object is currently embargoed indefinitely against IP ranges defined in the list: ') . $list . '.';
        $form['current_embargo'] = array(
          '#markup' => '<p>' . $embargo_message . '</p><hr/>',
        );
      }
      else {
        $year = date('Y', $embargo_info->expiry);
        $month = date('F', $embargo_info->expiry);
        $day = date('j', $embargo_info->expiry);
        $ordinal = date('S', $embargo_info->expiry);
        $embargo_message = t("This object is embargoed against IP ranges defined in the list: ") . $list . '. ' . t("The embargo is set to expire on the @day@ordinal of @month, @year.", array(
          '@day' => $day,
          '@month' => $month,
          '@year' => $year,
          '@ordinal' => $ordinal,
        ));
        $form['current_embargo_expiry'] = array(
          '#markup' => '<p>' . $embargo_message . '</p><hr/>',
        );
      }
    }
    $form['lists'] = array(
      '#type' => 'select',
      '#title' => 'IP address range list',
      '#description' => t('The list of IP ranges to limit access to this object to.'),
      '#options' => $options,
    );
    $form['expiry_date'] = array(
      '#type' => 'date',
      '#title' => 'Expiry',
      '#description' => t('When the Embargo will end.'),
    );
    $form['never_expire'] = array(
      '#type' => 'checkbox',
      '#title' => 'Never expire',
      '#description' => t('This embargo should never expire, overrides the Expiry date field.'),
    );

    if ($embargo_result->rowCount()) {
      $submit_button_value = t('Update Embargo');
      $form_state['storage']['islandora_ip_embargo']['update'] = TRUE;
      $form['lists']['#default_value'] = $embargo_info->lid;
      if (!isset($embargo_info->expiry)) {
        $form['never_expire']['#default_value'] = 1;
      }
      else {
        $year = date('Y', $embargo_info->expiry);
        $month = date('n', $embargo_info->expiry);
        $day = date('j', $embargo_info->expiry);
        $form['expiry_date']['#default_value'] = array(
          'year' => $year,
          'month' => $month,
          'day' => $day,
        );
      }
    }
    else {
      $submit_button_value = t('Set Embargo');
    }

    $form['submit'] = array('#type' => 'submit', '#value' => $submit_button_value);

    if ($embargo_result->rowCount()) {
      $form['delete'] = array(
        '#type' => 'submit',
        '#value' => 'Clear Embargo',
        '#submit' => array('islandora_ip_embargo_object_embargo_form_delete'),
        '#limit_validation_errors' => array(),
      );
    }
  }

  return $form;
}

/**
 * Submit handler for object embargoes.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_object_embargo_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  $expiry = NULL;
  if (!$form_state['values']['never_expire']) {
    $expiry = mktime(
      0,
      0,
      0,
      $form_state['values']['expiry_date']['month'],
      $form_state['values']['expiry_date']['day'],
      $form_state['values']['expiry_date']['year']
    );
  }
  if ($form_state['storage']['islandora_ip_embargo']['update']) {
    if ($form_state['values']['lists'] != 'none') {
      islandora_ip_embargo_update_embargo(
        $form_state['storage']['islandora_ip_embargo']['pid'],
        $form_state['values']['lists'],
        $expiry
      );
    }
  }
  else {
    islandora_ip_embargo_set_embargo(
      $form_state['storage']['islandora_ip_embargo']['pid'],
      $form_state['values']['lists'],
      $expiry
    );
  }
  return $form;
}

/**
 * Submit handler for object embargo deletion.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_object_embargo_form_delete($form, &$form_state) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  islandora_ip_embargo_remove_embargo($form_state['storage']['islandora_ip_embargo']['pid'], $is_pid = TRUE);
  return $form;
}
