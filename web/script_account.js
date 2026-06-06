let tblUserHeader = document.getElementById("tblUserHeader");
let btnSavePw = document.getElementById("btnSavePw");
let btnSaveUser = document.getElementById("btnSaveUser")
let tbNewUser = document.getElementById("tbNewUser");
let tbPwCurrent = document.getElementById("tbPwCurrent");
let tbPwNew = document.getElementById("tbPwNew");
let tbPwNewConfirm = document.getElementById("tbPwNewConfirm");
let tblLinkAdmin = document.getElementById("tblLinkAdmin");

let username;

onLoad();

async function onLoad()
{
    btnSavePw.addEventListener("click", btnSavePw_Click);
    btnSaveUser.addEventListener("click", btnSaveUser_Click);

    const response = await fetch("/api/account_page")

    if(response.status === 401)
    {
        window.location.replace("/signin.html")
    }  
    else if(response.status === 200)
    {
        const data = await response.json();
        username = data.username;
        if(data.admin == true) tblLinkAdmin.style.visibility = "visible";
        tblUserHeader.textContent=username;     
    }
}

async function btnSavePw_Click()
{
    if(tbPwNew.value === "") 
    {
        alert("Bitte gib ein neues Passwort ein");
        return;
    }

    if(tbPwNewConfirm.value !== tbPwNew.value) 
    {
        alert("Die eingegebenen Passwörter stimmen nicht überein!");
        return;
    }

    const data = {username: username, pw_old: tbPwCurrent.value, pw_new: tbPwNew.value}
    const result = await post_userapi("changepw", data);

    if(result.status === 401)
    {
        alert("Das eingegebene aktuelle Passwort ist falsch!")
    }
    else if(result.status === 200)
    {
        alert("Das Passwort wurde erfolgreich geändert!")
        window.location.replace("/signin.html")
    }
    else
    {
        alert("Unbekannter Fehler beim Ändern des Passworts!")
    }
}

async function btnSaveUser_Click()
{
    if(tbNewUser.value === "")
    {
        alert("Bitte gib einen neuen Benutzernamen ein!");
        return;
    }

    const data = {username_new: tbNewUser.value}
    await post_api("username-change", data);

    window.location.reload();
}