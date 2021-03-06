<?php 

function theme_cmis_browser($context) {
  // Render output
  drupal_add_css(drupal_get_path('module', 'cmis_browser') .'/cmis_browser.css');
  $contents = drupal_get_form('cmis_browser_browse_form');
  $contents .= drupal_get_form('cmis_browser_actions_form');
  $contents .= theme('cmis_browser_browse_breadcrumb', $context['bcarray']);
  $contents .= theme('cmis_browser_browse_children', $context);
  
  return $contents; 
}

/**
 * Custom theme for cmis_browser form.
 * 
 * @param $form
 */
function theme_cmis_browser_browse_form($form) {
  $header = array('', '');
  $rows = array(
    array(drupal_render($form['browse']['path']), drupal_render($form['browse']['submit'])),
  );

  return theme('table', $header, $rows) . drupal_render($form);
}

/**
 * Custom theme for cmis_browser_browse action
 * 
 * @param $children
 */
function theme_cmis_browser_browse_children($context = array()) {
  $header = array(t('name'), t('type'), t('size'), t('author'), t('last modified'), '');
  $rows = array();
  $folder_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/space.gif');
  $document_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/file.png');
  
  $children = $context['children']; 
  
  // adding up level link when lookup method is by id
  if (isset($context['id'])) {
    $folder_parent = end($context['parents']);
    if ($folder_parent->type == 'cmis:folder') {
      $folder_parent->title = '..';
      $children = array_merge(array($folder_parent), $children);
    }
  }
  
  foreach ($children as $child) {
    $author = $child->author;
    $updated = date_format($child->properties['lastModificationDate'], 'n/j/Y g:i A');
    $actions = array( 
      l(t('properties'), 'cmis/properties', array('attributes' => array('class' => 'action properties'), 'query' => array('id' => $child->id))),
      l(t('delete'), 'cmis/delete', array('query' => array('id' => $child->id, 'return_url' => $_GET['q'])))
    );
    
    // @todo should be baseType in place of type
    switch ($child->type) {
      case 'cmis:folder':
        $icon = $folder_img;
        if (isset($context['id'])) {
          $link = l($child->title, 'cmis/browser', array('query' => array('id' => $child->id)));
        }
        else {
          $link = l($child->title, implode('/', array_merge(array('cmis/browser'), $context['bcarray'], array($child->title))));
        }
        $mimetype = 'Space';
        $size = '';
        break;
      default:      
        $icon = $document_img;
        $link = l($child->title, 'cmis/browser', array('query' => array('id' => $child->id)));
        $mimetype = $child->contentMimeType;
        $size = number_format($child->size / 1000, 2, '.', ',') .' K';
    }
        
    $rows[] = array($icon .' '. $link, $mimetype, $size, $author, $updated, theme('item_list', $actions, NULL, 'ul', array('class'=>'actions')));
  }
  
  drupal_add_js('
    $(document).ready(function() {
      $("A.action.properties").each(function() {
        $(this).click(function() {
          $(this).parents("LI:first").toggleClass("expanded").toggleClass("collapsed");
          if ($(this).parents("TR:first").next().filter("TR.details").toggle().length == 0) {
            $("<td colspan=\"6\"><span class=\"load_indicator\">'. t('Loading'). '...</span></td>")
              .load(this.href+"&no_layout")
              .insertAfter($(this).parents("TR:first"))
              .wrapAll("<tr class=\"details\"></tr>")
              .before("<td></td>");
          }
          return false;
        }).parents("LI:first").toggleClass("collapsed");
      });
    });', 'inline');

  return theme('table', $header, $rows, array('class' => 'cmis_browser_browse_children')); 
}

/**
 * Theme for cmis_browser breadcrumb
 * 
 * @param $bcarray
 */
function theme_cmis_browser_browse_breadcrumb($bcarray) {
  $next_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/next.gif');
  
  $contents .= '<div id="cmis-breadcrumb">';
  $contents .= l('Root', 'cmis/browser'. $currentpath);
  
  $currentpath = '';
  foreach ($bcarray as $space) {
    $contents .= $next_img .' ';
    $currentpath .= '/'. $space;
    $pagelink = l($space, 'cmis/browser'. $currentpath);
    $contents .= $pagelink;    
  }

  $contents .= '</div>';

  return $contents;
}

/**
 * Generate CMIS document list view.
 * It displays document icon, name, download link, description, size, last modification date, 
 * modifier and thumbnail if any.
 * 
 * @param $target_path
 */
function theme_cmis_browser_doc_view($target_path) {
  module_load_include('api.inc', 'cmis');

  $folder_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/space.gif');
  $document_img = theme('image', drupal_get_path('module', 'cmis_browser') .'/images/file.png');
  
  try {
    $repository = cmisapi_getRepositoryInfo();
    $cmis_object = cmisapi_getProperties($repository->repositoryId, drupal_urlencode($target_path));
  } 
  catch (CMISException $e) {
    cmis_error_handler('cmis_browser', $e);
    return t('Errors occured while trying to access CMIS repository. Please check Drupal error logs. (@error)', array('@error' => $e->getMessage()));
  }
  
  $updated = date_format($cmis_object->updated, 'n/j/Y g:i A');
  if ($cmis_object->type == 'folder') {
    $icon = $folder_img;
    $link = l($cmis_object->title, 'cmis/browser'. $target_path);
  } 
  else {
    $icon = $document_img;
    $link = l($cmis_object->title, 'cmis/browser', array('query' => array('id' => $cmis_object->id)));    
  }
  
  $contents = '<div>'. $icon . $link .'</div>';
  $contents .= '<div>'. $cmis_object->summary .'</div>';
  $contents .= $cmis_object->type == 'folder'?'':'<div> Size:'. number_format($cmis_object->size/1000, 2, '.', ',') .' K</div>';
  $contents .= '<div> Modified:'. $updated .'</div>';
  $contents .= '<div> Modifier:'. $cmis_object->author .'</div>';
 
  return $contents;
}

/**
 * Theme for cmis_browser_content_properties action.
 * 
 * @param $cmis_object
 */
function theme_cmis_browser_content_properties($cmis_object) {
  $output = theme('box', $cmis_object->title, $cmis_object->summary);
  
  
  $header = array(t('Property'), t('Value'));
  $rows = array();
  
  $rows[] = array('<b>'. t('Author') .'</b>', $cmis_object->author);
  $rows[] = array('<b>'. t('Type') .'</b>', $cmis_object->author);
  
  foreach ($cmis_object->properties as $property => $value) {
    if ($value instanceof DateTime) {
      $rows[] = array('<b>'. $property.'</b>', date_format($value, 'n/j/Y g:i A'));
    }
    else {
      $rows[] = array('<b>'. $property.'</b>', $value);
    }

  }  
  
  return $output . theme('table', NULL, $rows);
}
