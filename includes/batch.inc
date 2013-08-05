<?php
/**
 * @file
 * Holds batch operations for the islandora_ip_embargo module.
 */

/**
 * This is a batch process for sending events based on embargoes being lifted.
 *
 * @param int $event_threshold
 *   Timestamp: anything expiring after this time will send out an event.
 * @param array $context
 *   The batch context.
 */
function islandora_ip_embargo_embargo_lift_event_batch($event_threshold, &$context) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');
  drupal_set_time_limit(240);

  // Set initial count.
  if (!isset($context['sandbox']['total_count'])) {
    $context['sandbox']['total_count'] = islandora_ip_embargo_count_embargoes_before_timestamp($event_threshold);
    $context['results']['event_threshold'] = $event_threshold;
    if ($context['sandbox']['total_count'] === 0) {
      $context['finished'] = 1;
      return;
    }
    $context['sandbox']['current'] = 0.0;
  }

  // Do the individual batch task.
  $results = islandora_ip_embargo_get_embargoes_before_timestamp($event_threshold, $context['sandbox']['current'], 10);
  $count = $results->rowCount();
  while ($data = $results->fetchObject()) {
    rules_invoke_event('islandora_ip_embargo_embargo_to_be_lifted', islandora_object_load($data->pid));
  }

  // Handle batch state.
  $context['sandbox']['current'] += $count;
  if ($count === 0) {
    $context['finished'] = 1;
  }
  else {
    $context['finished'] = $context['sandbox']['current'] / $context['sandbox']['total_count'];
  }
}

/**
 * The lift_event batch finished callback.
 *
 * Will update the database to indicate that the events fired.
 */
function islandora_ip_embargo_lift_event_batch_finished($success, $results, $operations) {
  module_load_include('inc', 'islandora_ip_embargo', 'includes/utilities');

  if (!$success) {
    drupal_set_message(t('An error occurred while sending embargo ending alerts'), 'error');
  }
  else {
    islandora_ip_embargo_set_event_triggered($results['event_threshold']);
  }
}
