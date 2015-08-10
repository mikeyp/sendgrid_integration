/**
 * Sendgrid Reports JS for creating graphs using Google Chart JS API
 */

(function ($) {
  Drupal.behaviors.sendgrid_integration_reports = {
    attach: function (context, settings) {
      google.load("visualization", "1", {
        packages: ["corechart"],
        "callback": drawCharts
      });

      function drawCharts() {
        var dataTableVol = new google.visualization.DataTable();
        dataTableVol.addColumn('datetime', Drupal.t('Date'));
        dataTableVol.addColumn('number', Drupal.t('Opens'));
        dataTableVol.addColumn('number', Drupal.t('Clicks'));
        dataTableVol.addColumn('number', Drupal.t('Delivered'));

        for (var key in settings.sendgrid_integration_reports.global) {
          dataTableVol.addRow([
            new Date(settings.sendgrid_integration_reports.global[key]['date']),
            settings.sendgrid_integration_reports.global[key]['opens'],
            settings.sendgrid_integration_reports.global[key]['clicks'],
            settings.sendgrid_integration_reports.global[key]['delivered']
          ]);
        }

        var options = {
          pointSize: 5,
          hAxis: {format: 'MM/dd/y'}
        };

        var chart = new google.visualization.LineChart(document.getElementById('sendgrid-global-volume-chart'));
        chart.draw(dataTableVol, options);
      }
    }
  }


})(jQuery);