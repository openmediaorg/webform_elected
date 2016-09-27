(function (window, Drupal, $) {
  var previousValues = {};

  Drupal.behaviors.webformElected = {
    attach: function (context, settings) {
      webformElectedSubmitEnabled();
      var keyComponentIds = settings.webformElected.keyComponentIds;
      $.each(keyComponentIds, function(index, keyComponentId){
        $('#' + keyComponentId).once('webformElected').each(function (index) {
          var $keyComponentField = $(this);
          var keyComponentType = settings.webformElected.keyComponentTypes[keyComponentId];
          var keyComponentHandler = Drupal.webformElected.keyComponentHandlers[keyComponentType];

          var originalValue = keyComponentHandler.getOriginalValue($keyComponentField);
          previousValues[keyComponentId] = originalValue ? originalValue : '';
          $keyComponentField.bind(keyComponentHandler.events, function () {
            validator($keyComponentField, keyComponentId, keyComponentHandler);
          });

          // Poll regularly to deal with autofill.
          // TODO make configurable: enable/disable and interval duration
          window.setInterval(function () {
            validator($keyComponentField, keyComponentId, keyComponentHandler);
          }, 1000);
        });
      });
    }
  };

  var validator = function($keyComponentField, keyComponentId, keyComponentHandler) {
    var keyValue = $keyComponentField.val();
    if (
      (previousValues[keyComponentId] != keyValue)
      &&
      keyComponentHandler.isValid(keyValue)
    ) {
      // Use custom event to avoid interference from browser-generated events like "input" or "change".
      $keyComponentField.trigger('webformElectedKeyComponentUpdated');
      previousValues[keyComponentId] = keyValue;
      return;
    }
    else {
      // TODO: Maybe we want to hide or clear the Representatives?
      // TODO: Maybe we should have a variable tracking valid/invalid
      //       postal code and if it changes, then do server reload
      // TODO: Invalid postal code should not allow for representative
      //       selection
    }
  };

  // We only have jQuery 1.4.x, so we have to devise our own way to get to the
  // contents of the HTML "value" attribute.
  // @see http://api.jquery.com/attr/
  // @see http://api.jquery.com/prop/
  var getValueViaDomAttribute = function($inputField) {
    var element = $inputField.get(0);
    if ('object' == typeof element) {
      if ('attributes' in element) {
        var attributes = element.attributes;
        for (var i = attributes.length - 1; i >= 0; i--) {
          if ('value' == attributes[i].name) {
            return attributes[i].value;
          }
        }
      }
    }
    return null;
  }

  var getSelectedOptionValueViaDomAttribute = function($inputField) {
    var $selectedOption = $('option[selected]', $inputField);
    return getValueViaDomAttribute($selectedOption);
  }

  // TODO provide these keyComponentHandlers via plugins
  Drupal.webformElected = {};
  Drupal.webformElected.keyComponentHandlers = {
    'postal_code': {
      getOriginalValue: function ($inputField) {
        return getValueViaDomAttribute($inputField);
      },
      isValid: function (value) {
        var postalCodeValidation = /^[a-zA-Z]\d[a-zA-Z]\s?\d[a-zA-Z]\d$/; // Only Canadian Postal Code so far.
        return (
          (value.length > 5)
          &&
          postalCodeValidation.test(value)
        );
      },
      events: 'input'
    },
    'mep_constituency': {
      getOriginalValue: function ($inputField) {
        return ''; // Hack to trigger AJAX for first value as well.
        // return getSelectedOptionValueViaDomAttribute($inputField);
      },
      isValid: function (value) {
        return (2 == value.split('--').length);
      },
      events: 'input change'
    }
  };

  function webformElectedSubmitEnabled() {
    var disabled = false;
    var keyComponentIds = Drupal.settings.webformElected.keyComponentIds;
    $.each(keyComponentIds, function(index, keyComponentId){
      if (!$('#' + keyComponentId).val()) {
        disabled = true;
      }
    });
    var representNames = Drupal.settings.webformElected.representNames;
    $.each(representNames, function(index, representName){
      if ($(':radio[name=' + representName + ']:checked').val()) {
        disabled = true;
      }
    });
    if (disabled) {
      $('input[type="submit"]').attr('disabled', true);
      $('input[type="submit"]').addClass('form-button-disabled');
    }
    else {
      $('input[type="submit"]').removeAttr('disabled');
      $('input[type="submit"]').removeClass('form-button-disabled');
    }
  }

}(window, Drupal, jQuery));
