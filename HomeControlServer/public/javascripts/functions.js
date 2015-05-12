function post2node(path,select_id, target_iframe, method) {
    method = method || "post"; // Set method to post by default if not specified.    
	var e = document.getElementById(select_id);
	var switchstate = e.options[e.selectedIndex].value;									
	var params=[];
	params['switchid']=select_id;
	params['state']=switchstate;                        	
    var form = document.createElement("form");
    form.setAttribute("method", method);
    form.setAttribute("action", path);
    form.setAttribute("target", target_iframe);

    for(var key in params) {
        if(params.hasOwnProperty(key)) {
            var hiddenField = document.createElement("input");
            hiddenField.setAttribute("type", "hidden");
            hiddenField.setAttribute("name", key);
            hiddenField.setAttribute("value", params[key]);
            form.appendChild(hiddenField);
         }
    }

    document.body.appendChild(form);
    form.submit();
}


//example : post('/contact/', {name: 'Johnny Bravo'});