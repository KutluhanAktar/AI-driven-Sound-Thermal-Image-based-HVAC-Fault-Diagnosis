// Every 5 seconds, retrieve the HTML elements (sections) generated from the system log updates registered on the MariaDB database table.
setInterval(function(){
	$.ajax({
		url: "./assets/dashboard_updates.php?new_update",
		type: "GET",
		success: (response) => {
			// Decode the obtained JSON object.
			const data = JSON.parse(response);
			// Assign the fetched HTML elements (sections) as the most recent system log updates to the web dashboard home (index) page.
			$(".log_updates").html(data.generated_html_elements);
			// According to the passed display option, show the associated system log updates on the dashboard — home page.
			if(current_display_option == 1){ $(".t_sample").hide(); $(".a_sample").hide(); }
			if(current_display_option == 2){ $(".t_detection").hide(); $(".a_sample").hide(); }
			if(current_display_option == 3){ $(".t_detection").hide(); $(".t_sample").hide(); }
		}
	});
}, 5000);

// Change the currently selected display category option according to the clicked horizontal menu button.
var current_display_option = -1; 
$(".category_menu").on("click", "button", event => {
	$(".category_menu button").removeClass("active");
	$(event.target).addClass("active");
	current_display_option = event.target.id;
});