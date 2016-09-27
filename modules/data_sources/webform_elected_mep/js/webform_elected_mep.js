(function (window, document, $, Drupal) {
  if (! ('settings' in Drupal)) {
    Drupal.settings = {};
  }
  if (! ('webformElectedMepDefault' in Drupal.settings)) {
    Drupal.settings.webformElectedMepDefault = {};
  }

  Drupal.webformElectedMepSetDefaults = function (elementName, country, constituency) {
    Drupal.settings.webformElectedMepDefault[elementName] = {country:country, constituency:constituency};
  }

  var defaults = function($constituencies) {
    var elementName = $constituencies.attr('name');
    return Drupal.settings.webformElectedMepDefault[elementName];
  };

  var settings = function() {
    return Drupal.settings.webformElectedMep || {countries:{},constituencies:{}};
  };

  var countConstituencies = function () {
    if (
      ('constituencyCounts' in settings())
      &&
      ('Germany' in settings().constituencyCounts)
    ) {
      return;
    }

    settings().constituencyCounts = {};
    $.each(settings().constituencies, function (countryValue, constituencies) {
      var count = 0;
      $.each(constituencies, function (constituencyValue, constituencyLabel) {
        count++;
      });
      settings().constituencyCounts[countryValue] = count;
    });
  };

  var setOptions = function ($select, options, defaultOptionValue) {
    $('option', $select).remove();
    var index = 0;
    $.each(options, function (optionValue, optionLabel) {
      var selected = '';
      if (
        (optionValue === defaultOptionValue)
        ||
        (index === defaultOptionValue)
      ) {
        selected = ' selected';
      }
      $select.append('<option value="' + optionValue + '"' + selected + '>' + optionLabel + '</option>');
      index++;
    });
  };

  var countryChangeHandler = function ($countries, $constituencies) {
    var countryValue = $countries.val();
    var constituencyOptions = settings().constituencies[countryValue];
    if (1 < settings().constituencyCounts[countryValue]) {
      $constituencies.show();
      setOptions($constituencies, constituencyOptions, defaults($constituencies).constituency);
    }
    else {
      setOptions($constituencies, constituencyOptions, 0);
      $constituencies.hide();
    }
    $constituencies.change();
  };

  Drupal.behaviors.webformElectedMep = {
    attach: function (context, drupalSettings) {
      countConstituencies(); // Will count only once.
      $('.webform-elected-mep').once('webformElectedMep').each(function () {
        var $countries = $('.webform-elected-mep-countries', this);
        var $constituencies = $('.webform-elected-mep-constituencies', this);
        // setOptions($countries, settings().countries, defaults($constituencies).country);
        $countries.change(function () {countryChangeHandler($countries, $constituencies)});
        $countries.change();
      });
    }
  };
})(window, document, jQuery, Drupal);
