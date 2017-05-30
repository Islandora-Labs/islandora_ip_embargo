<?php
/**
 * @file
 * Drush interface to ip_embargo functionality.
 */

/**
 * Drush commands declaration.
 *
 * @return array
 *   drush commands data
 */
function islandora_ip_embargo_drush_command() {
  $items = array();
  $items['islandora-ip-embargo-addRange'] = array(
    'description' => 'Add a network address range to the embargo list.',
    'options' => array(
      'low' => array(
        'description' => 'First address in the IP range (e.g, "172.16.0.0").',
        'required' => 'TRUE',
      ),
      'high' => array(
        'description' => 'Last address in the IP range (e.g., "172.16.255.255").',
        'required' => 'TRUE',
      ),
      'list' => array(
        'description' => 'Name of the list to add this range to (will be created if necessary).',
        'required' => 'TRUE',
      ),
    ),
    'aliases' => array('iipeadd'),
  );
  $items['islandora-ip-embargo-list-lists'] = array(
    'description' => 'Returns all the network lists defined on the system.',
    'aliases' => array('iipell'),
  );
  $items['islandora-ip-embargo-set-embargo-for-item'] = array(
    'description' => 'Set an embargo on an islandora object.',
    'options' => array(
      'pid' => array(
        'description' => 'Islandora pid to embargo.',
        'required' => 'TRUE',
      ),
      'list' => array(
        'description' => 'Name of the IP list to use for the embargo.',
        'required' => TRUE,
      ),
      'expiry' => array(
        'description' => 'Date of expiry.',
        'value' => 'optional',
      ),
    ),
    'aliases' => array('iipese'),
  );
  $items['islandora-ip-embargo-set-embargo-from-pid-list'] = array(
    'description' => dt('Create a collection container.'),
    'examples' => array('drush islandora-utils-create-collection --parent=islandora:root --namespace=my:collection'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
    'drupal dependencies' => array(
      'islandora',
      // Include version dependency ?
      'islandora_ip_embargo',
    ),
    'options' => array(
      'pidsfile' => array(
        'description' => dt('File containing a comma-separated string of pids'),
        'required' => FALSE,
      ),
      'lists' => array(
        'description' => dt('comma-separated names of network lists to set.'),
        'required' => TRUE,
      ),
      'expiry' => array(
        'description' => dt("When to expire the embargo."),
        'required' => FALSE,
      ),
    ),
  );
  return $items;
}

/**
 * Drush command; adds IP range to a named list.
 *
 * Creates the list if it does not already exist.
 *
 * @return NULL
 *   no return
 */
function drush_islandora_ip_embargo_addRange() {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  $params = array(
    'low' => drush_get_option('low'),
    'high' => drush_get_option('high'),
    'list' => drush_get_option('list'),
  );

  $low = $params['low'];
  $high = $params['high'];
  // Check that input vars are valid IPs.
  foreach (array($low, $high) as $ip) {
    if (filter_var($ip, FILTER_VALIDATE_IP) === FALSE) {
      drush_set_error("Invalid IP: $ip");
      return;
    }
  }

  $target_list = $params['list'];
  $target_list_id = FALSE;
  $lists = islandora_ip_embargo_get_lists();

  foreach ($lists as $list) {
    if ($target_list == $list->name) {
      $target_list_id = $list->lid;
      if (is_duplicate_range($list->lid, $low, $high)) {
        return;
      }
      break;
    }
  }
  if (!$target_list_id) {
    islandora_ip_embargo_add_list($target_list);
    $target_list_id = islandora_ip_embargo_get_list_object_by('name', $target_list)->lid;
    drupal_set_message(t('Created list %l', array('%l' => $target_list)));
  }

  islandora_ip_embargo_add_ip_range($target_list_id, $low, $high);
}

/**
 * Check that record to be created is not already stored.
 *
 * @param int $lid
 *   list ID
 * @param string $low
 *   IPV4 address that starts the allowed range.
 * @param string $high
 *   IPV4 address that ends the allowed range.
 *
 * @return bool
 *   true if an existing record matches the one being tested.
 */
function is_duplicate_range($lid, $low, $high) {
  foreach (islandora_ip_embargo_get_ranges($lid) as $range) {
    if ($range->low_end == $low && $range->high_end == $high) {
      drupal_set_message(t("Duplicate IP range. Nothing changed."));
      return TRUE;
    }
  }
}

/**
 * Drush command; prints named lists and their constituent ranges to stdOut.
 */
function drush_islandora_ip_embargo_list_lists() {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');

  $lists = islandora_ip_embargo_get_lists();
  printf("Network lists\n==============\n");
  foreach ($lists as $list) {
    printf("%s\n", $list->name);
    foreach (islandora_ip_embargo_get_ranges($list->lid) as $range) {
      printf("%s - %s\n", $range->low_end, $range->high_end);
    }
    printf("\n");
  }
}

/**
 * Drush command; creates an embargo for the given pid.
 */
function drush_islandora_ip_embargo_set_embargo_for_item() {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  $params = array(
    'pid' => drush_get_option('pid'),
    'list' => drush_get_option('list'),
    'expiry' => drush_get_option('expiry'),
  );
  make_embargo_from_params($params);
}

/**
 * Drush command; given a list of pids, embargo each.
 *
 * Similar to function drush_islandora_ip_embargo_set_embargo_for_item;
 */
function drush_islandora_ip_embargo_set_embargo_from_pid_list() {
  $pids = explode(',', file_get_contents(drush_get_option('pidsfile')));
  $expiry = drush_get_option('expiry');
  $lists = explode(',', drush_get_option('lists'));
  $params = array('expiry' => $expiry);

  foreach ($lists as $list) {
    foreach ($pids as $pid) {
      $params['pid'] = $pid;
      $params['list'] = $list;
      make_embargo_from_params($params);
    }
  }
}

/**
 * Create an embargo based on the input params.
 *
 * @param array $params
 *   key => value for keys (list,exiry,pid)
 *
 * @return NULL
 *   no return
 */
function make_embargo_from_params($params) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  $pid = $params['pid'];
  $list = islandora_ip_embargo_get_list_object_by('name', $params['list']);

  // Set up expiry date from params.
  if ($params['expiry'] !== NULL) {
    $date_elements = explode('-', $params['expiry']);
    if (count($date_elements) != 3) {
      drush_set_error("Expiry argument expects the date given as YYYY-MM-DD. Got $date_elements");
      return;
    }
    $expiry_elements = array(
      'year' => $date_elements[0],
      'month' => $date_elements[1],
      'day' => $date_elements[2],
    );
    $expiry = mktime(
        0, 0, 0, $expiry_elements['month'], $expiry_elements['day'], $expiry_elements['year']
    );
  }

  // Check for good pid.
  $object = islandora_object_load($pid);
  if (!$object) {
  }
  $exp_str = $expiry == NULL ? "Never" : $expiry;

  // Determine whether an embargo exists already for an object.
  $update = islandora_ip_embargo_get_embargo($pid)->fetchObject();
  if ($update) {
    if ($update->lid == $list->lid && $update->expiry == $expiry) {
      drupal_set_message(t(
          "No update required for IP range list %list to Islandora object %obj, set to expire %exp",
          array('%list' => $params['list'], '%obj' => $pid, '%exp' => $exp_str))
      );
    }
    islandora_ip_embargo_update_embargo($pid, $list->lid, $expiry);
    drupal_set_message(
        t(
            "Updated IP range list %list to Islandora object %obj, set to expire %exp",
            array(
              '%list' => $params['list'],
              '%obj' => $pid,
              '%exp' => $exp_str,
            )
        )
    );
    watchdog(
        'islandora_ip_embargo',
        "Updated IP range list %list to Islandora object %obj, set to expire %exp",
        array(
          '%list' => $params['list'],
          '%obj' => $pid,
          '%exp' => $exp_str,
        ),
        WATCHDOG_NOTICE
    );
  }
  else {
    islandora_ip_embargo_set_embargo($pid, $list->lid, $expiry);
    drupal_set_message(
        t(
            "Added IP range list %list to Islandora object %obj, set to expire %exp",
            array(
              '%list' => $params['list'],
              '%obj' => $pid,
              '%exp' => $exp_str,
            )
        )
    );
    watchdog(
        'islandora_ip_embargo',
        "Added IP range list %list to Islandora object %obj, set to expire %exp",
        array(
          '%list' => $params['list'],
          '%obj' => $pid,
          '%exp' => $exp_str,
        ),
        WATCHDOG_NOTICE
    );
  }
}