class Group
{
    constructor(id, team1, team2, team3, team4) 
    {
        this.id = id;
        this.teams = [team1, team2, team3, team4];
    }

    up(team)
    {
        const i = this.teams.findIndex(t => t === team);
        if(i === 0) return;

        const temp = this.teams[i - 1];
        this.teams[i - 1] = team;
        this.teams[i] = temp;
    }

    down(team)
    {
        const i = this.teams.findIndex(t => t === team);
        if(i === 3) return;

        const temp = this.teams[i + 1];
        this.teams[i + 1] = team;
        this.teams[i] = temp;
    }
}

let divGroups = document.getElementById("divGroups");
let tblLinkAdmin = document.getElementById("tblLinkAdmin");
let groups = [];

onLoad();

async function onLoad()
{
    const response = await fetch("/api/group-page");
    if(response.status === 400)
    {
        alert("Interner Fehler!");     
    }
    else if(response.status === 401)
    {
        window.location.replace("/signin.html");
    }
    else if(response.status === 200)
    {
        const data = await response.json();

        tblUserHeader.textContent = data.username;
        if(data.admin == true) tblLinkAdmin.style.visibility = "visible";

        data.groups.forEach(e => {
            const teams = [];
            const id = e.group;
            e.teams.forEach(i => {
                const pos = i.prediction - 1;
                if(pos === -2)
                {
                    teams.push(i.team);
                }
                else
                {
                    teams[i.prediction - 1] = i.team;
                }
            });

            addGroup(id, teams[0], teams[1], teams[2], teams[3]);
        });
    }
}

function addGroup(id, team1, team2, team3, team4)
{
    const g = new Group(id, team1, team2, team3, team4);
    const divGroup = tplGroup.content.cloneNode(true);
    divGroup.querySelector("#tblGroup").textContent = `Gruppe ${id}`;
    divGroup.querySelector("#divGroup").id = `divGroup_${id}`;

    let i = 1;
    g.teams.forEach(element => {
        divGroup.querySelector("#divTeams").appendChild(createTeamDiv(divGroup, i, g, element));
        i++;
    });

    groups.push(g);
    divGroups.appendChild(divGroup);
}

function createTeamDiv(divGroup, num, group, team)
{
    const div = tplTeam.content.cloneNode(true);

    div.querySelector("#tblNumber").textContent = num;
    div.querySelector("#tblTeam").textContent = team === "Bosnien-Herzegowina" ? "Bosnien-H." : team;
    div.querySelector("#imgTeam").src = `https://flagcdn.com/${countryDict[team]}.svg`

    const btnSavePred = divGroup.querySelector("#btnSavePred");
    btnSavePred.dataset.id = group.id;
    btnSavePred.addEventListener("click", btnSavePred_Click);

    div.querySelector("#btnUp").addEventListener("click", () => {
        group.up(team);
        btnSavePred.style.visibility = "visible";
        refreshDisplay(group.id)
    })
    div.querySelector("#btnDown").addEventListener("click", () => {
        group.down(team);
        btnSavePred.style.visibility = "visible";
        refreshDisplay(group.id)
    })

    return div;
}

function refreshDisplay(id)
{
    const divGroup = divGroups.querySelector(`#divGroup_${id}`);
    const divTeams = divGroup.querySelector("#divTeams");
    divTeams.innerHTML = "";

    const g = getGroup(id);

    let i = 1;
    g.teams.forEach(element => {
        divTeams.appendChild(createTeamDiv(divGroup, i, g, element));
        i++;
    });
}

function getGroup(id)
{
    return groups.find(e => e.id === id);
}

async function btnSavePred_Click(event)
{
    const id = event.currentTarget.dataset.id;
    const g = getGroup(id);

    let i = 1;
    for (const e of g.teams) 
    {
        const data = { teamname: e, prediction: i };
        const response = await post_api("place-group-bet", data, false);

        if(response.status !== 200)
        {
            alert(`Beim Speichern ist ein Fehler aufgetreten: ${response.status}`);
            return;
        }
        i++;
    }

    alert("Die Vorhersage wurde erfolgreich gespeichert!");
}