let loginButton = document.getElementById("login_button");
let cookieButton = document.getElementById("cookie_button");
let cookieBox = document.getElementById("cookie_box");

onLoad();


function onLoad()
{
    loginButton.addEventListener("click", function() {
        alert("Der Login ist aktuell noch nicht verfügbar.");
    });

        cookieButton.addEventListener("click", function() {
        cookieBox.style.visibility="hidden";
    });
}