let tabUsers = document.getElementById("tabUsers");
let tblUserHeader = document.getElementById("tblUserHeader");
let tbAddUserName = document.getElementById("tbAddUserName");
let tbAddUserPw = document.getElementById("tbAddUserPw");
let btnAddUser = document.getElementById("btnAddUser");
let tblLinkAdmin = document.getElementById("tblLinkAdmin");
let tbScore1 = document.getElementById("tbScore1");
let tbScore2 = document.getElementById("tbScore2");
let tbMatchId = document.getElementById("tbMatchId");
let btnUpdateMatch = document.getElementById("btnUpdateMatch");
let btnUpdatePoints = document.getElementById("btnUpdatePoints");

onLoad();

async function onLoad()
{
    btnAddUser.addEventListener("click", btnAddUser_Click);
    btnUpdateMatch.addEventListener("click", btnUpdateMatch_Click);
    btnUpdatePoints.addEventListener("click", btnUpdatePoints_Click);

    const response = await fetch("/api/dashboard-page");

    if(response.status === 401)
    {
        window.location.replace("/signin.html")
    }
    else if(response.status === 200)
    {
        const data = await response.json();
        tblUserHeader.textContent = data.username;
        if(data.admin == 1) tblLinkAdmin.style.visibility = "visible";

        //Display user list
        const users = data.user_list;
        users.forEach(element => {
            addUserToTable(element.id, element.username, element.admin);
        });
    }
}

function addUserToTable(id, name, isAdmin)
{
        const row = tabUsers.insertRow();
        const idCell = row.insertCell();
        const nameCell = row.insertCell();
        const pwCell = row.insertCell();
        const permCell = row.insertCell();
        const deleteCell = row.insertCell();

        //Id
        idCell.textContent=id;

        //Username
        const tbUserName = document.createElement("input");
        tbUserName.type = "text";
        tbUserName.id = `tbUserName_${id}`;
        tbUserName.value=name;
        tbUserName.style.color="white";
        tbUserName.style.backgroundColor="rgb(103, 56, 103)";
        nameCell.appendChild(tbUserName);

        const btnSaveUser = document.createElement("button");
        btnSaveUser.textContent="Speichern";
        btnSaveUser.style.marginLeft="10px";
        btnSaveUser.dataset.id=id;
        btnSaveUser.addEventListener("click", btnSaveUser_Click);
        nameCell.appendChild(btnSaveUser);

        //Password
        const tbResetPw = document.createElement("input");
        tbResetPw.type = "password";
        tbResetPw.style.color="white";
        tbResetPw.style.backgroundColor="rgb(103, 56, 103)";
        tbResetPw.id = `tbResetPw_${id}`;
        pwCell.appendChild(tbResetPw);

        const btnResetPw = document.createElement("button");
        btnResetPw.textContent="Überschreiben";
        btnResetPw.style.marginLeft="10px";
        btnResetPw.dataset.id = id;
        btnResetPw.addEventListener("click", btnResetPw_Click);
        pwCell.appendChild(btnResetPw);

        //Permissions
        const cbIsAdmin = document.createElement("input");
        cbIsAdmin.type = "checkbox";
        cbIsAdmin.checked = isAdmin;
        cbIsAdmin.id = `cbIsAdmin_${id}`;
        cbIsAdmin.textContent = "Admin";
        
        const lblIsAdmin = document.createElement("label");
        lblIsAdmin.textContent="Admin"
        lblIsAdmin.prepend(cbIsAdmin);
        permCell.appendChild(lblIsAdmin);   

        const btnSavePerms = document.createElement("button");
        btnSavePerms.textContent="Speichern";
        btnSavePerms.style.marginLeft="10px";
        btnSavePerms.dataset.id = id;
        btnSavePerms.addEventListener("click", btnSavePerms_Click);
        permCell.appendChild(btnSavePerms);

        //Delete
        const btnDeleteUser = document.createElement("button");
        btnDeleteUser.textContent="Löschen";
        btnDeleteUser.style.marginLeft="10px";
        btnDeleteUser.dataset.id = id;
        btnDeleteUser.addEventListener("click", btnDeleteUser_Click);
        deleteCell.appendChild(btnDeleteUser);
}

async function btnAddUser_Click()
{
    if(tbAddUserPw.value === "" || tbAddUserName.value === "")
    {
        alert("Bitte gib einen Benutzernamen und ein Passwort für den neuen Benutzer ein!");
        return;
    }

    const data = {username_new: tbAddUserName.value, password: tbAddUserPw.value}

    const response = await post_api("user-add", data);

    window.location.reload();
}

async function btnResetPw_Click(event)
{
    const id = parseInt(event.currentTarget.dataset.id, 10);
    const tbResetPw = document.getElementById(`tbResetPw_${id}`);

    if(tbResetPw.value === "")
    {
        alert("Bitte gib ein neues Passwort an!");
        return;
    }


    const data = {"user_id": id, "password_new": tbResetPw.value};
    const response = await post_api("user-set-password", data);
}

async function btnSaveUser_Click(event)
{
    const id = parseInt(event.currentTarget.dataset.id, 10);
    const tbUserName = document.getElementById(`tbUserName_${id}`);

    if(tbUserName.value === "")
    {
        alert("Bitte gib einen neuen Benutzernamen an!");
        return;
    }


    const data = {"user_id": id, "username_new": tbUserName.value};
    const response = await post_api("user-set-name", data);
}

async function btnSavePerms_Click(event)
{
    const id = parseInt(event.currentTarget.dataset.id, 10);
    const cbIsAdmin = document.getElementById(`cbIsAdmin_${id}`);

    let isAdmin;
    switch(cbIsAdmin.checked)
    {
        case true:
            isAdmin = 1;
            break;
        case false:
            isAdmin = 0;
            break;
    }

    const data = {"user_id": id, "admin": isAdmin};
    const response = await post_api("user-set-admin", data);
}

async function btnDeleteUser_Click(event)
{
    const id = parseInt(event.currentTarget.dataset.id, 10);
    const confirmDel = confirm("Möchtest du den Benutzer wirklich löschen?");

    if(!confirmDel) return;

    const data = {"user_id": id};
    const response = await post_api("user-remove", data);

    window.location.reload();
}

async function btnUpdateMatch_Click()
{
    if(tbMatchId.value === "" || tbScore1.value === "" || tbScore2.value === "")
    {
        alert("Bitte fülle alle erforderlichen Felder für das Aktualisieren eines Matches aus");
        return;
    }

    const data = {"id": parseInt(tbMatchId.value), "score1": parseInt(tbScore1.value), "score2": parseInt(tbScore2.value)}
    console.log(data);
    const response = await post_api("set-match-score", data);

    window.location.reload();
}

async function btnUpdatePoints_Click()
{
    const response = await post_api("update-points", {});
}