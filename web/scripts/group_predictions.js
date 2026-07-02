let tabPredictions = document.getElementById("tabPredictions");
let trPredictions = document.getElementById("trPredictions");
let group;
let team;

onLoad();

async function onLoad()
{
    const queryString = window.location.search;  
    const urlParams = new URLSearchParams(queryString);  

    group = urlParams.get("group");

    const data = {group: group};

    const response = await post_api("group-preds-page", data, false);
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

        tblContentHeader.textContent = `Gewählte Gruppe: ${group}`;

        teams = [data.team1, data.team2, data.team3, data.team4];
        
        //List teams in header
        for(i = 0; i < 4; i++)    
        {
            const th = document.createElement("th");
            th.textContent = teams[i];
            trPredictions.appendChild(th);
        }

        //Points header
        const th = document.createElement("th");
        th.textContent = "Punkte";
        trPredictions.appendChild(th);

        //Predictions
        const predictions = data.predictions;
        if(typeof predictions !== "undefined")
        {
            console.log(predictions);

            predictions.forEach(element => {
                addEntryToTable(element.username, element.pred1, element.pred2, element.pred3, element.pred4);
            });
        }
        else //If the match has not started or doesn't exist, show error
        {
            tblContentHeader.textContent = "Ungültige Gruppe";
            tblContentDesc.textContent = "Die ausgewählte Gruppe existiert nicht.";
            tabPredictions.style.visibility = "hidden";
        }
    };
}

function addEntryToTable(username, pred1, pred2, pred3, pred4)
{
    const row = tabPredictions.insertRow();
    const cellUsername = row.insertCell();

    const preds = [pred1, pred2, pred3, pred4];
    const isValid = !hasDuplicates(preds); //If one of the preds is duplicate it means that the person forgot to guess and should be displayed as all zeros. This is only a sloppy fix as its still handled weirdly on the server side. But it should work for now.

    let points = 0;

    for(i = 0; i < 4; i++)
    {
        const cell = row.insertCell();
        const a = preds.findIndex((e) => e === teams[i]); //Find the index of the team in the predictions to get the predicted pos

        cell.textContent = isValid ? a + 1 : 0;

        //Show color for wrong/right
        cell.style.fontWeight = "bold";
        if(a === i && isValid)
        {
            cell.style.color = "green";
            points += 2;
        }
        else
        {
            cell.style.color = "red";
        }
    }

    const cellPoints = row.insertCell();
    cellPoints.textContent = points;
    cellUsername.textContent = username;
}