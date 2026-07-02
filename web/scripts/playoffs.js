let divMatches = document.getElementById("divMatches");
let tplMatch = document.getElementById("tplMatch");
let tblUserHeader = document.getElementById("tblUserHeader");
let tblLinkAdmin = document.getElementById("tblLinkAdmin");

let days = [];
let isAdmin;
let scroll = 0;

onLoad();

async function onLoad()
{
    btnUp.addEventListener("click", () => window.scrollTo({top: 0}));

    const response = await fetch("/api/playoffs-page");
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
       
        tblUserHeader.textContent = data.username;
        isAdmin = data.admin;
        tblPoints.textContent = `${data.points} Punkte`;
        if(isAdmin == true) tblLinkAdmin.style.visibility = "visible";

        data.matches.forEach(e => {
            addMatch(e.id, e.group, e.time, e.team1, e.team2, e.prediction1, e.prediction2, e.score1, e.score2, e.predicted_winner, e.winner);
        });

        //Scroll to the current day
        scrollTo({top: scroll});
    };
}

function addMatch(id, group, dateTime, team1, team2, pred1, pred2, score1, score2, predWin, finalWin)
{
    const dateTimeObj = new Date(dateTime);
    const time = `${String(dateTimeObj.getHours()).padStart(2, "0")}:${String(dateTimeObj.getMinutes()).padStart(2, "0")} Uhr`;
    const day = `${String(dateTimeObj.getDate()).padStart(2, "0")}.${String(dateTimeObj.getMonth() + 1).padStart(2, "0")}.${dateTimeObj.getFullYear()}`;
    const timeDiff = dateTimeObj.getTime() - Date.now();

    //If the day is not already on the board, create a new header
    if(!days.includes(day))
    {
        const tblDay = document.createElement("p");
        tblDay.textContent=day;
        tblDay.classList="font_large";
        divMatches.appendChild(tblDay);
        days.push(day);

        if(isCurrentDay(dateTime))
        {
            //Save scroll offset for when the page is done loading
            scroll = tblDay.getBoundingClientRect().top - 20;
        }
    }

    const div = tplMatch.content.cloneNode(true);

    //Show id if account is admin
    if(isAdmin)
    {
        div.querySelector("#tblTime").textContent=`${time} - ID: ${id}`;
    }
    else
    {
        div.querySelector("#tblTime").textContent=time;
    }

    //Display the correct scores and the appropriate color
    let tblScore1 = div.querySelector("#tblScore1");
    let tblScore2 = div.querySelector("#tblScore2");
    if(score1 != -1) tblScore1.textContent=score1;
    if(score2 != -1) tblScore2.textContent=score2;

    if(score1 > score2)
    {
        tblScore1.style.color = "green";
        tblScore2.style.color = "red";
    }
    else if(score1 < score2)
    {
        tblScore1.style.color = "red";
        tblScore2.style.color = "green";
    }

    //Flags
    div.querySelector("#imgTeam1").src = `https://flagcdn.com/${countryDict[team1]}.svg`;
    div.querySelector("#imgTeam2").src = `https://flagcdn.com/${countryDict[team2]}.svg`;
    div.querySelector("#tblMatch").textContent = `${team1} vs. ${team2}`

    //Display the predictions
    let tblPred1 =  div.querySelector("#tblPred1");
    let tblPred2 = div.querySelector("#tblPred2");
    let cbxWinner = div.querySelector("#cbxWinner");
    tblPred1.textContent = Math.max(pred1,0);
    tblPred1.id = `tblPred1_${id}`;
    tblPred2.textContent = Math.max(pred2,0);
    tblPred2.id = `tblPred2_${id}`;

    //Setup combobox for selecting winner
    let optTeam1 = document.createElement("option");
    let optTeam2 = document.createElement("option");
    optTeam1.textContent = team1;
    optTeam2.textContent = team2;
    cbxWinner.add(optTeam1);
    cbxWinner.add(optTeam2);
    cbxWinner.selectedIndex = predWin;
    cbxWinner.id = `cbxWinner_${id}`;

    let btnSavePred = div.querySelector("#btnSavePred");
    btnSavePred.addEventListener("click", btnSavePred_Click)
    btnSavePred.dataset.id = id;

    //Event handlers to update score and show "Save" button
    div.querySelector("#btnPred1Up").addEventListener("click", () => {
        tblPred1.textContent = (parseInt(tblPred1.textContent) + 1);
        btnSavePred.style.visibility = "visible";
    });
    div.querySelector("#btnPred2Up").addEventListener("click", () => {
        tblPred2.textContent = (parseInt(tblPred2.textContent) + 1);
        btnSavePred.style.visibility = "visible";
    });
    div.querySelector("#btnPred1Down").addEventListener("click", () => {
        tblPred1.textContent = (Math.max(0, parseInt(tblPred1.textContent) - 1));
        btnSavePred.style.visibility = "visible";
    });
    div.querySelector("#btnPred2Down").addEventListener("click", () => {
        tblPred2.textContent = (Math.max(0, parseInt(tblPred2.textContent) - 1));
        btnSavePred.style.visibility = "visible";
    });
    div.querySelector(`#cbxWinner_${id}`).addEventListener("click", () => {
        btnSavePred.style.visibility = "visible";
    });


    //If the match is already ongoing or done, hide entry controls and show points
    if(timeDiff < 0)
    {
        const divBottomRow = div.querySelector("#divBottomRow");
        divBottomRow.removeChild(div.querySelector("#btnSavePred"));

        div.querySelector("#btnPred1Up").style.visibility = "hidden";
        div.querySelector("#btnPred2Up").style.visibility = "hidden";
        div.querySelector("#btnPred1Down").style.visibility = "hidden";
        div.querySelector("#btnPred2Down").style.visibility = "hidden";
        div.querySelector(`#cbxWinner_${id}`).disabled = true;

        //Add link to page that shows other players predictions
        div.querySelector("#divMatch").addEventListener("click", () => window.location.assign(`/match_predictions.html?match_id=${id}`));
        div.querySelector("#divMatch").style.cursor="pointer";


        if((score1 != -1 && score2 != -1))
        {
            const tblPoints = div.querySelector("#tblPoints");
            tblPoints.style.visibility = "visible";
            tblPoints.textContent = `+ ${calcPoints(id, pred1, pred2, score1, score2, predWin, finalWin)} Punkte`;
        }
    }

    divMatches.appendChild(div);
}

async function btnSavePred_Click(event)
{
    const id = event.currentTarget.dataset.id;
    let tblPred1 = document.getElementById(`tblPred1_${id}`);
    let tblPred2 = document.getElementById(`tblPred2_${id}`);
    let cbxWinner = document.getElementById(`cbxWinner_${id}`);

    if(tblPred1.textContent === "" || tblPred2.textContent === "")
    {
        alert("Bitte gib für beide Teams eine Vorhersage ab!");
        return;
    }

    const data = {"id":  parseInt(id), "prediction1": parseInt(tblPred1.textContent), "prediction2": parseInt(tblPred2.textContent), "predicted_winner": parseInt(cbxWinner.selectedIndex)};
    const response = await post_api("place-bet-playoffs", data);

    if(response.status === "200") btnSavePred.style.visibility = "hidden";
}