<?php

namespace Drupal\sendgrid_integration\Form;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class SendGridSettingsForm
 * @package Drupal\sendgrid_integration\Form
 */
class SendGridSettingsForm extends ConfigFormBase {

  /**
   * The module handler service.
   *
   * @var \Drupal\Core\Extension\ModuleHandlerInterface
   */
  protected $moduleHandler;

  /**
   * Constructs a \Drupal\system\ConfigFormBase object.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The factory for configuration objects.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $moduleHandler
   *   The module handler service.
   */
  public function __construct(ConfigFactoryInterface $config_factory, ModuleHandlerInterface $moduleHandler) {
    parent::__construct($config_factory);
    $this->moduleHandler = $moduleHandler;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('config.factory'),
      $container->get('module_handler')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'sendgrid_integration_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['sendgrid_integration.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('sendgrid_integration.settings');

    $form['authentication'] = [
      '#type' => 'fieldset',
      '#title' => $this->t('Authentication'),
    ];

    $requirenewkey = TRUE;
    if (!empty($config->get('apikey'))) {
      $form['authentication']['secretkeynotice'] = [
        '#markup' => $this->t('You have saved a secret key. You may change the key by inputing a new one in the field directly below.'),
      ];
      $requirenewkey = FALSE;
    }

    $form['authentication']['sendgrid_integration_apikey'] = [
      '#type' => 'password',
      '#required' => $requirenewkey,
      '#title' => $this->t('API Secret Key'),
      '#description' => $this->t('The secret key of your key pair. These are only generated once by Sendgrid. Your existing key is hidden. If you need to change this, provide a new key here.'),
    ];

    $form['debugging']['maillog'] = [
      '#type' => 'fieldset',
      '#title' => $this->t('Maillog integration'),
    ];

    if (!$this->moduleHandler->moduleExists('maillog')) {
      $form['debugging']['maillog']['#description'] = $this->t('Installing the <a href="@url">Maillog module</a> also allows keeping copies of all emails sent through the site.', ['@url' => 'https://www.drupal.org/project/maillog']);
    }
    else {
      $form['debugging']['maillog']['#description'] = $this->t('The <a href="@url">Maillog module</a> is installed, it can also be used to keep copies of all emails sent through the site.', ['@url' => Url::fromRoute('maillog.settings')->toString()]);

      $form['debugging']['maillog']['sendgrid_integration_maillog_log'] = [
        '#type' => 'checkbox',
        '#title' => $this->t('Create table entries in maillog table for each e-mail.'),
        '#default_value' => $config->get('maillog_log'),
      ];

      $form['debugging']['maillog']['sendgrid_integration_maillog_devel'] = [
        '#type' => 'checkbox',
        '#title' => $this->t('Display the e-mails on page using devel module (if enabled).'),
        '#default_value' => $config->get('maillog_devel'),
        '#disabled' => !$this->moduleHandler->moduleExists('devel'),
      ];
    }

    return parent::buildForm($form, $form_state);
  }

  /**
   * [@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('sendgrid_integration.settings');
    // Check for API secret key. If missing throw error.
    if (empty($config->get('apikey')) && empty($form_state->getValue('sendgrid_integration_apikey'))) {
      $form_state->setError($form['authentication']['sendgrid_integration_apikey'], $this->t('You have not stored an API Secret Key.'));
    }
    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('sendgrid_integration.settings');

    if ($form_state->hasValue('sendgrid_integration_maillog_log')) {
      $config->set('maillog_log', $form_state->getValue('sendgrid_integration_maillog_log'));
    }
    if ($form_state->hasValue('sendgrid_integration_maillog_devel')) {
      $config->set('maillog_devel', $form_state->getValue('sendgrid_integration_maillog_devel'));
    }

    if ($form_state->hasValue('sendgrid_integration_apikey') && !empty($form_state->getValue('sendgrid_integration_apikey'))) {
      $config->set('apikey', $form_state->getValue('sendgrid_integration_apikey'));
    }

    $config->save();
    parent::submitForm($form, $form_state);
  }

}
