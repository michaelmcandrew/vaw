
function webfm_popup() {}


//read GET parameter from the url
//for example on the url http://example.com/page.php?some=value&more=evenmore&get_it=yes
//calling get_url_param('get_it') would return 'yes'
function get_url_param(name) {
	var regexS, regex, results;
	name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
	regexS = "[\\?&]" + name + "=([^&#]*)";
	regex = new RegExp(regexS);
	results = regex.exec(window.location.href);
	if (results === null) {
		return "";
	}
	else {
		return results[1];
	}
}

//Connect wysiwyg module and TinyMCE
//as described in http://wiki.moxiecode.com/index.php/TinyMCE:Custom_filebrowser
//This function opens the webfm_popup
function webfm_popupTinyMCEConnector(field_name, url, type, win) {
	Drupal.settings.wysiwygDialogWindow = win;
	window.open(Drupal.settings.basePath + '?q=webfm_popup&url=' + field_name + '&caller=tinymce', 'ImageBrowser', 'width=750,height=500,toolbar=0,resizable=1,location=0,status=0,menubar=0,scrollbars=1');
}

//This Function allows sending the image to one of the many wysiwyg editors
webfm_popup.sendtocaller = function (obj) {
	var fid, result_url, doc, url_id, fpath, webfm_id;
	fid = $('#' + obj.row_id).find('td').eq(1).find('a').attr('title');
	//gettng the actual url (path alias) for the file.
	//need to remove the quotes created by responseText
	result_url = $.ajax({
		type: "GET",
		url: Drupal.settings.basePath,
		data: { q: '/webfm_popup/get_path_alias/webfm_send/' + fid },
		async: false 
	}).responseText.replace(/["]/g, '');

	
	switch (get_url_param('caller')) {
    case 'ckeditor':
    	window.opener.CKEDITOR.tools.callFunction(get_url_param('CKEditorFuncNum'), result_url);
    	break;
    case 'tinymce':
		doc = $(window.opener.Drupal.settings.wysiwygDialogWindow.document);
		url_id = '#' + get_url_param('url');
		doc.find(url_id).val(result_url);
		break;
    case 'fckeditor':
		// the window this popup was called from
		doc = $(window.opener.document);
		// put the file url in the input with the id specified in the url
		fpath = obj.filepath;
		url_id = '#' + get_url_param('url');
		doc.find(url_id).val(Drupal.settings.basePath + Drupal.settings.webfm_popup.fileDirectory + fpath);
		// put the webfm file-id in the input with the id specified in the url
		webfm_id = '#' + get_url_param('webfmid');
		doc.find(webfm_id).val(result_url);
		break;
	}
	
	window.opener.focus();
	window.close();
	return;
};

//Add the send to rich text editor link to the right click menu of files.
function webfm_popupGetMenusAjax() {
	Webfm.menuHT.put('file', new Webfm.menuElement(Drupal.t("Send to rich text editor"), webfm_popup.sendtocaller, ""));
}

if (Drupal.jsEnabled) {
	$(window).load(function () {
		if (typeof(Webfm) !== 'undefined' && Webfm.menuHT !== null) {
			webfm_popupGetMenusAjax();
		}
	}
	);
}