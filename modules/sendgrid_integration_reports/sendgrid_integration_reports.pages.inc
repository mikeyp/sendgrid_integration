<?php

/**
 * @file
 * Provides reports pages for Sendgrid Reports module.
 */

/**
 * Global stats page.
 */
function sendgrid_integration_reports_global() {
  $stats = sendgrid_integration_reports_get_stats();
  $settings = array();
  foreach ($stats['global']['volume'] as $items) {
    $settings['sendgrid_integration_reports']['global'][] = array(
      'date' => $items['date'],
      'opens' => $items['opens'],
      'clicks' => $items['clicks'],
      'delivered' => $items['delivered'],
    );
  }
  $path = drupal_get_path('module', 'sendgrid_integration_reports');
  $render = array(
    '#attached' => array(
      'js' => array(
        array(
          'data' => 'https://www.google.com/jsapi',
          'type' => 'external',
        ),
        $path . '/sendgrid_integration_reports.js',
        array(
          'data' => $settings,
          'type' => 'setting',
        ),
      ),
    ),
    'message' => array(
      '#markup' => t(
        'The following reports are based on Mandrill data from the last 30 days. For more comprehensive data, please visit your !dashboard. !cache to ensure the data is current.',
        array(
          '!dashboard' => l(t('Mandrill Dashboard'), 'https://mandrillapp.com/'),
          '!cache' => l(t('Clear your cache'), 'admin/config/development/performance'),
        )
      ),
    ),
    'volume' => array(
      '#prefix' => '<h2>' . t('Sending Volume') . '</h2>',
      '#markup' => '<div id="sendgrid-global-volume-chart"></div>',
    ),
  );

  return $render;
}

