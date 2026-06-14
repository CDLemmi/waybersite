let tabLeaderboard = document.getElementById("tabLeaderboard");

onLoad();

async function onLoad()
{
    const response = await fetch("/api/leaderboard-page")

    if(response.status === 401)
    {
        window.location.replace("/signin.html")
    }  
    else if(response.status === 200)
    {
        const data = await response.json();
        username = data.username;
        tblPoints.textContent = `${data.points} Punkte`;
        if(data.admin == true) tblLinkAdmin.style.visibility = "visible";
        tblUserHeader.textContent=username;
        
        const leaderboard = data.leaderboard;
        leaderboard.forEach(element => {
            addUserToTable(element.name, element.points);
        });
    }
}

function addUserToTable(name, points)
{
    const row = tabLeaderboard.insertRow();

    const nameCell = row.insertCell();
    nameCell.textContent = name;

    const pointCell = row.insertCell();
    pointCell.textContent = points;
}