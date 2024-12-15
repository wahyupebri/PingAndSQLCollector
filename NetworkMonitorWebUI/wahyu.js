$(document).ready(function(){
	function performAjaxRequest() {
		$.ajax({
			url: "http://127.0.0.1:8080/update",//EDIT THIS!
			type: "GET",
			dataType: "json",
			success: function(data) {
				console.log("Received JSON data:", data);
				for (var i = 0; i < data.length; i++) {
					var id = data[i].id;
					var value = data[i].value;
					var el=document.getElementById(id);
					if(value<0){
						el.classList.remove('green');
						if(value==-1){
							el.classList.add('red');
							el.classList.remove('grey');
						}	
						else if(value==-2){
							el.classList.add('grey');
							el.classList.remove('red');
						}
						else{
							el.classList.remove('grey');
							if((value/60)>-5){
								el.classList.remove('red');
								el.classList.remove('orange');
								el.classList.add('yellow');
							}
							else if((value/60)>-30){
								el.classList.remove('red');
								el.classList.remove('yellow');
								el.classList.add('orange');
							}
							else{
								el.classList.remove('orange');
								el.classList.add('red');
								el.classList.remove('yellow');
							}
						}	
					}
					else if(value>=0){
						el.classList.remove('red');
						el.classList.remove('grey');
						el.classList.remove('orange');
						el.classList.remove('yellow');
						el.classList.add('green');
					}	
				}
				 setTimeout(performAjaxRequest, 3000);
			},
			error: function(jqXHR, textStatus, errorThrown) {
				console.error("Error:", textStatus, errorThrown);
				setTimeout(performAjaxRequest, 4000);
			}
			 
		});
	}
	performAjaxRequest();
});