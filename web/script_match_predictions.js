let tabPredictions = document.getElementById("tabPredictions");

let score1 = 0;
let score2 = 0;

onLoad();

async function onLoad()
{
    const queryString = window.location.search;  
    const urlParams = new URLSearchParams(queryString);  

    const data = {match_id: parseInt(urlParams.get("match_id"))};

    const response = await post_api("match-preds-page", data, false);
    if(response.status === 401)
    {
        window.location.replace("/signin.html")
    }
    else if(response.status === 400)
    {
        alert("Interner Serverfehler.")
    }
    else if(response.status === 200)
    {
        const data = await response.json();
       
        //Default page content
        tblUserHeader.textContent = data.username;
        isAdmin = data.admin;
        tblPoints.textContent = `${data.points} Punkte`;
        if(isAdmin == true) tblLinkAdmin.style.visibility = "visible";

        //Match specific stuff
        score1 = data.score1;
        score2 = data.score2;

        tblContentHeader.textContent = `Gewähltes Match: ${data.team1} vs. ${data.team2}`;

        //Predictions
        const predictions = data.predictions;
        predictions.forEach(element => {
            addEntryToTable(element.user, element.pred1, element.pred2);
        });

        console.log(data);
    };
}


function addEntryToTable(user, pred1, pred2)
{
    const row = tabPredictions.insertRow();
    const nameCell = row.insertCell();
    const scoreCell = row.insertCell();
    const pointsCell = row.insertCell();

    nameCell.textContent = user;

    if(pred1 != -1 && pred2 != -1)
    {
        scoreCell.textContent = `${pred1} : ${pred2}`;
        pointsCell.textContent = calcPoints(pred1, pred2, score1, score2)
    }
    else
    {
        scoreCell.textContent = "-";
        pointsCell.textContent = "0";
    }

}