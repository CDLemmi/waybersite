let tabPredictions = document.getElementById("tabPredictions");
let tblContentHeader = document.getElementById("tblContentHeader");
let tblContentDesc = document.getElementById("tblContentDesc");

let match_id = 0;
let score1 = 0;
let score2 = 0;
let winner = -1;
let team1 = "";
let team2 = "";

onLoad();

async function onLoad()
{
    const queryString = window.location.search;  
    const urlParams = new URLSearchParams(queryString);  

    match_id = parseInt(urlParams.get("match_id"));

    const data = {match_id: match_id};

    const response = await post_api("match-preds-page", data, false);
    if(response.status === 401)
    {
        window.location.replace("/signin.html")
    }
    else if(response.status === 400)
    {
        showNotification("Fehler 400", "Interner Serverfehler");
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
        winner = data.winner;
        team1 = data.team1;
        team2 = data.team2;

        tblContentHeader.textContent = `Gewähltes Match: ${data.team1} vs. ${data.team2}`;
        if(data.score1 != -1 && data.score2 != -1)
        {
            //If the match has already concluded, also show score
            tblContentHeader.textContent += ` (${data.score1} : ${data.score2})`

            //If the match is a playoffs match, also show winner
            if(match_id >= 73) tblContentHeader.textContent += data.winner === 0 ? ` (Gewinner: ${data.team1})` : ` (Gewinner: ${data.team2})` 
        } 

        //Predictions
        const predictions = data.predictions;
        if(typeof predictions !== "undefined")
        {
            predictions.forEach(element => {
                addEntryToTable(element.user, element.pred1, element.pred2, element.predWin);
            });
        }
        else //If the match has not started or doesn't exist, show error
        {
            tblContentHeader.textContent = "Ungültiges Match";
            tblContentDesc.textContent = "Das ausgewählte Spiel existiert nicht oder steht noch bevor.";
            tabPredictions.style.visibility = "hidden";
        }
    };
}


function addEntryToTable(user, pred1, pred2, predWin)
{
    const row = tabPredictions.insertRow();
    const nameCell = row.insertCell();
    const scoreCell = row.insertCell();
    const pointsCell = row.insertCell();

    nameCell.textContent = user;
    scoreCell.textContent = `${pred1} : ${pred2}`;

    pointsCell.textContent = "-"
    if(score1 != -1 && score2 != -1)
    {
        pointsCell.textContent = calcPoints(match_id, pred1, pred2, score1, score2, predWin, winner)
    }

    if(match_id >= 73) //If playoff match, also show predicted winner
    {
        let thPred = document.getElementById("thPred");
        let predWinString = predWin == 0 ? team1 : team2;

        thPred.style.width = "250px";
        scoreCell.textContent += `  (Gewinner: ${predWinString})`
    }
}