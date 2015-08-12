<?php

/**
 * @file
 * Provides reports pages for Sendgrid Reports module.
 */

/**
 * Global stats page.
 */
function sendgrid_integration_reports_global() {
  $stats = sendgrid_integration_reports_get_stats_global();
  $settings = array();
  foreach ($stats['global'] as $items) {
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
        'The following reports are the from the Global Statistics provided by Sendgrid. For more comprehensive data, please visit your !dashboard. !cache to ensure the data is current.',
        array(
          '!dashboard' => l(t('Sendgrid Dashboard'), 'https://app.sengrid.com/'),
          '!cache' => l(t('Clear your cache'), 'admin/config/development/performance'),
        )
      ),
    ),
    'volume' => array(
      '#prefix' => '<h2>' . t('Sending Volume') . '</h2>',
      '#markup' => '<div id="sendgrid-global-volume-chart"></div>',
    ),
    'spam' => array(
      '#prefix' => '<h2>' . t('Spam Reports') . '</h2>',
      '#markup' => '<div id="sendgrid-global-spam-chart"></div>',
    ),
  );

  return $render;
}

