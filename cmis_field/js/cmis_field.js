(function($){

  Drupal.behaviors.cmisField = {
    attach: function(context, settings) {
      var rootDir = Drupal.settings.cmispath;
      var cmisActive = '';
      //The buttons
      $("input.cmis-field-setting").each(function(){
    	  $(this).after(' <input type="button" id="' + $(this).attr('id') + '-button" class="cmis-field-path form-submit" value="Browse" />');
      });
      $("input.cmis-field").each(function(){
    	  $(this).after(' <input type="button" id="' + $(this).attr('id') + '-button" class="cmis-field-file form-submit" value="Browse" />');
      });
      //The dialog box
      if($("input.cmis-field").length > 0) {
    	  $('<div id="cmis-browser-dialog" style="display:none">loading...</div>').dialog({autoOpen: false, width: 600, title: "CMIS-Browser"});
      }
      $("form").delegate("input.cmis-field-path", "click", function(event){
    	cmisActive = $('#' + $(this).attr('id').replace('-button', ''));
	    $('#cmis-browser-dialog').load('/cmis/browser' + rootDir + '?type=popup&caller=settings').dialog('open');
    	event.preventDefault();
      });
      $("form").delegate("input.cmis-field-file", "click", function(event){
    	cmisActive = $('#' + $(this).attr('id').replace('-button', ''));
  	    $('#cmis-browser-dialog').load('/cmis/browser' + rootDir + '?type=popup&caller=widget').dialog('open');
      	event.preventDefault();
      });
      $('#cmis-browser-dialog a').live("click", function(event){
		if($(this).hasClass('cmis-field-insert')) {
			if(cmisActive.hasClass('cmis-field-setting')) {
	          var cmispath = $(this).attr('href');
	          cmisActive.val(cmispath.replace("//", "/"));
	        }
	        else {
	          cmisActive.val($(this).attr('id'));
	          $('#' + cmisActive.attr('id').replace('-path', '-title')).val($(this).attr('name'));
	        }
	        jQuery('#cmis-browser-dialog').dialog("close");
		}
		else {
		  $('#cmis-browser-dialog').load($(this).attr('href'));  
		}
    	event.preventDefault();
      });
    }
  };
  
})(jQuery);