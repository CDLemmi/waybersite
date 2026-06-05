let divMatches = document.getElementById("divMatches");
let tplMatch = document.getElementById("tplMatch");
let tblUserHeader = document.getElementById("tblUserHeader");
let tblLinkAdmin = document.getElementById("tblLinkAdmin");

let days = [];

onLoad();

async function onLoad()
{
    const response = await fetch("/api/match-page");
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
        if(data.admin == true) tblLinkAdmin.style.visibility = "visible";

        data.matches.forEach(e => {
            addMatch(e.id, e.group, e.time, e.team1, e.team2, e.prediction1, e.prediction2, e.score1, e.score2);
        });
    };
}

function addMatch(id, group, dateTime, team1, team2, pred1, pred2, score1, score2)
{
    const dateTimeObj = new Date(dateTime);
    const time = `${String(dateTimeObj.getHours()).padStart(2, "0")}:${String(dateTimeObj.getMinutes()).padStart(2, "0")} Uhr`;
    const day = `${String(dateTimeObj.getDate()).padStart(2, "0")}.${String(dateTimeObj.getMonth() + 1).padStart(2, "0")}.${dateTimeObj.getFullYear()}`;
    const timeDiff = dateTimeObj.getTime() - Date.now();

    if(!days.includes(day))
    {
        const tblDay = document.createElement("p");
        tblDay.textContent=day;
        tblDay.classList="font_large";
        divMatches.appendChild(tblDay);
        days.push(day);
    }

    const div = tplMatch.content.cloneNode(true);
    div.querySelector("#tblTime").textContent=time;

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

    div.querySelector("#imgTeam1").src = `https://flagcdn.com/${countryDict[team1]}.svg`;
    div.querySelector("#imgTeam2").src = `https://flagcdn.com/${countryDict[team2]}.svg`;
    div.querySelector("#tblMatch").textContent = `${team1} vs. ${team2}`

    let tblPred1 =  div.querySelector("#tblPred1");
    let tblPred2 = div.querySelector("#tblPred2");
    tblPred1.textContent = Math.max(pred1,0);
    tblPred1.id = `tblPred1_${id}`;
    tblPred2.textContent = Math.max(pred2,0);
    tblPred2.id = `tblPred2_${id}`;

    let btnSavePred = div.querySelector("#btnSavePred");
    btnSavePred.addEventListener("click", btnSavePred_Click)
    btnSavePred.dataset.id = id;

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

    if(timeDiff < 0)
    {
        div.querySelector("#btnPred1Up").style.visibility = "hidden";
        div.querySelector("#btnPred2Up").style.visibility = "hidden";
        div.querySelector("#btnPred1Down").style.visibility = "hidden";
        div.querySelector("#btnPred2Down").style.visibility = "hidden";
        div.querySelector("#btnSavePred").style.visibility = "hidden";
    }

    divMatches.appendChild(div);
}

async function btnSavePred_Click(event)
{
    const id = event.currentTarget.dataset.id;
    let tblPred1 = document.getElementById(`tblPred1_${id}`);
    let tblPred2 = document.getElementById(`tblPred2_${id}`);

    if(tblPred1.textContent === "" || tblPred2.textContent === "")
    {
        alert("Bitte gib für beide Teams eine Vorhersage ab!");
        return;
    }

    const data = {"id":  parseInt(id), "prediction1": parseInt(tblPred1.textContent), "prediction2": parseInt(tblPred2.textContent)};
    const response = await post_api("place-bet", data);

    if(response.status === "200") btnSavePred.style.visibility = "visible";
}