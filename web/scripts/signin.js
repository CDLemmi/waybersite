let btnLogin = document.getElementById("btnLogin");
let tbUsername = document.getElementById("tbUsername");
let tbPassword = document.getElementById("tbPassword");
let cbSaveLogin = document.getElementById("cbSaveLogin");

onLoad();

function onLoad()
{
    btnLogin.addEventListener("click", btnLogin_Click);
}

async function btnLogin_Click()
{
    if(tbPassword.value === "" || tbUsername.value === "") 
    {
        showNotification("Fehler", "Bitte gib einen Benutzernamen und ein Passwort ein");
        return;
    }

    const saveLogin = cbSaveLogin.checked == true ? 1 : 0; 
    const body = { username: tbUsername.value, password: tbPassword.value, save_session: saveLogin};
    const response = await post_userapi("signin", body);
    
    if(response.status === 401)
    {
        showNotification("Fehler", "Falsche Kombination aus Benutzername und Passwort");
    }
    else if(response.status === 200)
    {
        window.location.assign("/home.html");
    }
    else
    {
        showNotification("Fehler", "Unbekannter Fehler beim Anmelden");
    }
}