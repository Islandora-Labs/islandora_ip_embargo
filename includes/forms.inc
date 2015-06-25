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
    '#description' => t('The URL to go to when an object is embargoed.'),
    '#default_value' => variable_get(
      'islandora_ip_embargo_embargoed_redirect',
      ''
    ),
  );
  $form['islandora_ip_embargo_embargoed_redirect_append_url'] = array(
    '#type' => 'checkbox',
    '#title' => t("Append embargoed object's URL."),
    '#default_value' => variable_get('islandora_ip_embargo_embargoed_redirect_append_url', 0),
    '#description' => t("Check this option if you want to append the embargoed object's URL
      before redirecting (for example, for use with Ezproxy)."),
  );
  $form['islandora_ip_embargo_days_before_embargo_trigger'] = array(
    '#type' => 'textfield',
    '#title' => t('Days before embargo alert'),
    '#description' => t('Rules will be triggered this number of days before an embargo is lifted.'),
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

    if (!is_object($fedora_object)) {
      continue;
    }

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
    $options[$data->lid]['name'] = l($data->name, 'admin/islandora/tools/ip_embargo/lists/' . $data->lid);
  }
  $header = array('name' => t('Name'));
  $form['Lists'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#attributes' => array(),
    '#empty' => t("There are no network lists."),
  );
  $form['pager'] = array(
    '#markup' => theme('pager'),
  );

  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  $form['Add'] = array(
    '#markup' => l(t('Add network list'), 'admin/islandora/tools/ip_embargo/lists/add'),
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
        form_set_error('list', t("You can't delete a network list with embargoes."));
      }
      $ranges_results = islandora_ip_embargo_get_lists_information($list_identifier);
      if ($ranges_results->rowCount()) {
        form_set_error('list', t("You can't delete a network list with ranges."));
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
    '#title' => t('Network List Name'),
    '#description' => t('Label for the list of IP addresses.'),
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
  $form_state['redirect'] = 'admin/islandora/tools/ip_embargo';
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
  $form_state['storage']['islandora_ip_embargo']['list_identifier'] = $list_identifier;

  $header = array('low_end' => t('low end'), 'high_end' => t('high end'));
  $options = array();
  while ($data = $list_information->fetchObject()) {
    $options[$data->rid] = array(
      'high_end' => $data->high_end,
      'low_end' => $data->low_end,
    );
  }

  $form['name'] = array(
    '#title' => t('Network List Name'),
    '#description' => t('Label for the list of IP addresses.'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $list_name,
  );
  $form['name_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update name'),
    '#submit' => array('islandora_ip_embargo_edit_list_form_name_submit'),
  );
  $form['ranges'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#attributes' => array(),
    '#empty' => t("There are no ranges."),
  );
  $form['pager'] = array(
    '#markup' => theme('pager'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete ranges'),
    '#submit' => array('islandora_ip_embargo_edit_list_form_range_submit'),
  );
  $form['Add'] = array(
    '#markup' => l(t('Add IP address range'), "admin/islandora/tools/ip_embargo/ranges/add/$list_identifier"),
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
    '#title' => t('Low end'),
    '#description' => t('First address in the IP range (e.g, "172.16.0.0").'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );
  $form['high_end'] = array(
    '#title' => t('High end'),
    '#description' => t('Last address in the IP range (e.g., "172.16.255.255").'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );
  $form_state['storage']['islandora_ip_embargo']['list_identifier'] = $list_identifier;
  $form['submit'] = array('#type' => 'submit', '#value' => t('Add IP address range'));
  return $form;
}

/**
 * Validate handler for adding an IP Range.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_ip_embargo_add_range_form_validate(array $form, array &$form_state) {
  $comparable_low = ip2long($form_state['values']['low_end']);
  $comparable_high = ip2long($form_state['values']['high_end']);

  if ($comparable_low === FALSE) {
    form_set_error('low_end', t('Low end IP is not valid.'));
  }

  if ($comparable_high === FALSE) {
    form_set_error('high_end', t('High end IP is not valid.'));
  }

  if ($comparable_low !== FALSE && $comparable_high !== FALSE &&  $comparable_low > $comparable_high) {
    form_set_error('', t('IP range is not valid. The low address is greater than the high.'));
  }
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

  $form_state['redirect'] = "admin/islandora/tools/ip_embargo/lists/$list_identifier";
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
  $form_state['storage']['islandroa_ip_embargo']['update'] = FALSE;
  $form_state['storage']['islandroa_ip_embargo']['pid'] = $islandora_object->id;
  $list_results = islandora_ip_embargo_get_lists();
  $options = array('none' => NULL);
  while ($data = $list_results->fetchObject()) {
    $options[$data->lid] = $data->name;
  }

  $form['lists'] = array(
    '#type' => 'select',
    '#title' => 'IP address range list',
    '#description' => t('The list of IP ranges for which this object will be available.  Any other network address will be blocked.'),
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

  $embargo_result = islandora_ip_embargo_get_embargo($islandora_object->id);
  if ($embargo_result->rowCount()) {
    $embargo_info = $embargo_result->fetchObject();
    $form_state['storage']['islandroa_ip_embargo']['update'] = TRUE;
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

  $form['submit'] = array('#type' => 'submit', '#value' => t('Set Embargo'));

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
  if ($form_state['storage']['islandroa_ip_embargo']['update']) {
    if ($form_state['values']['lists'] != 'none') {
      islandora_ip_embargo_update_embargo(
        $form_state['storage']['islandroa_ip_embargo']['pid'],
        $form_state['values']['lists'],
        $expiry
      );
    }
    else {
      islandora_ip_embargo_remove_embargo($form_state['storage']['islandroa_ip_embargo']['pid']);
    }
  }
  else {
    islandora_ip_embargo_set_embargo(
      $form_state['storage']['islandroa_ip_embargo']['pid'],
      $form_state['values']['lists'],
      $expiry
    );
  }
  return $form;
}
