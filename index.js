var map;
var x;
function loadmaps() {
  $.getJSON(
    'https://api.thingspeak.com/channels/1769802/fields/1/last.json?api_key=8EC1TO1SG37E7USO',
    function (result) {
      var m = result;
      x = Number(m.field1);
      //alert(x);
    }
  );
  $.getJSON(
    'https://api.thingspeak.com/channels/1769802/fields/2/last.json?api_key=8EC1TO1SG37E7USO',
    function (result) {
      var m = result;
      y = Number(m.field2);
    }
  ).done(function () {
    initialize();
  });
}

window.setInterval(function () {
  loadmaps();
}, 9000);
function initialize() {
  //alert(y);
  var mapOptions = {
    zoom: 18,
    center: { lat: x, lng: y },
  };
  map = new google.maps.Map(document.getElementById('map'), mapOptions);

  var marker = new google.maps.Marker({
    position: { lat: x, lng: y },
    map: map,
  });

  var infowindow = new google.maps.InfoWindow({
    content: '<p>Marker Location:' + marker.getPosition() + '</p>',
  });

  google.maps.event.addListener(marker, 'click', function () {
    infowindow.open(map, marker);
  });
}

google.maps.event.addDomListener(window, 'load', initialize);

// ('use strict');

// $(document).ready(function () {
//   $(window).bind('scroll', function (e) {
//     parallaxScroll();
//   });
// });

// function parallaxScroll() {
//   const scrolled = $(window).scrollTop();
//   $('#team-image').css('top', 0 - scrolled * 0.2 + 'px');
//   $('.img-1').css('top', 0 - scrolled * 0.35 + 'px');
//   $('.img-2').css('top', 0 - scrolled * 0.05 + 'px');
// }
