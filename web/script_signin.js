let btnLogin = document.getElementById("btnLogin");
let tbUsername = document.getElementById("tbUsername");
let tbPassword = document.getElementById("tbPassword");

onLoad();

function onLoad()
{
    btnLogin.addEventListener("click", btnLogin_Click);
}

async function btnLogin_Click()
{
    if(tbPassword.value === "" || tbUsername.value === "") 
    {
        alert("Bitte gib einen Benutzernamen und ein Passwort ein");
        return;
    }

    const body = { username: tbUsername.value, password: tbPassword.value};
    const response = await post_userapi("signin", body);
    
    if(response.status === 401)
    {
        alert("Falsche Kombination aus Benutzername und Passwort");
    }
    else if(response.status === 200)
    {
        window.location.assign("/account.html");
    }
    else
    {
        alert("Unbekannter Fehler beim Anmelden.")
    }
}