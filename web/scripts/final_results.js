let tabLeaderboard = document.getElementById("tabLeaderboard");
let imgPodium = document.getElementById("imgPodium");

onLoad();

async function onLoad()
{
    const response = await fetch("/api/final-results-page")

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
        
        //Show final leaderboard results
        const leaderboard = data.leaderboard;
        i = 1;
        leaderboard.forEach(element => {
            addUserToTable(i, element.name, element.points);
            i++;
        });

        //Load podium image
        const response2 = await fetch("/content/final_podium.png");
        const image = await response2.blob();
        imgPodium.src = URL.createObjectURL(image);
    }
}

function addUserToTable(num, name, points)
{
    const row = tabLeaderboard.insertRow();

    const numCell = row.insertCell();
    numCell.textContent = num;
    if(num === 1)
    {
        numCell.style.color = "gold";
    }
    else if(num === 2)
    {
        numCell.style.color = "silver";
    }
    else if(num === 3)
    {
        numCell.style.color = "brown";
    }

    const nameCell = row.insertCell();
    nameCell.textContent = name;

    const pointCell = row.insertCell();
    pointCell.textContent = points;
}