let countryDict = {
    Mexiko: "mx",
    Südafrika: "za",
    Südkorea: "kr",
    Tschechien: "cz",
    "Bosnien Herzegowina": "ba",
    Kanada: "ca",
    Katar: "qa",
    Schweiz: "ch",
    Brasilien: "br",
    Haiti: "ht",
    Marokko: "ma",
    Schottland: "gb-sct",
    Australien: "au",
    Paraguay: "py",
    Türkei: "tr",
    USA: "us",
    Curacao: "cw",
    Deutschland: "de",
    Ecuador: "ec",
    Elfenbeinküste: "ci",
    Japan: "jp",
    Niederlande: "nl",
    Schweden: "se",
    Tunesien: "tn",
    Ägypten: "eg",
    Belgien: "be",
    Iran: "ir",
    Neuseeland: "nz",
    "Kap Verde": "cv",
    "Saudi-Arabien": "sa",
    Spanien: "es",
    Uruguay: "uy",
    Frankreich: "fr",
    Irak: "iq",
    Norwegen: "no",
    Senegal: "sn",
    Algerien: "dz",
    Argentinien: "ar",
    Jordanien: "jo",
    Österreich: "at",
    Kolumbien: "co",
    "DR Kongo": "cd",
    Portugal: "pt",
    Usbekistan: "uz",
    England: "gb-eng",
    Ghana: "gh",
    Kroatien: "hr",
    Panama: "pa"
};

let userBoxTimeout = null;
let userBox = null;

onLoad();

function onLoad()
{
    displayCookieBox();

    if(document.getElementById("tblUserHeader"))
    {
        tblUserHeader.addEventListener("mouseenter", () => {
        clearTimeout(userBoxTimeout);
        if (!userBox) {
            userBox = getUserBox();
            document.body.appendChild(userBox); 
            }
        });
        tblUserHeader.addEventListener("mouseleave", () => {
            scheduleHideUserbox();
        });
    }

}

async function post_userapi(api, body)
{
    return await fetch(`/userapi/${api}`, {
        method: "POST",
        body: JSON.stringify({body}),
        headers: 
        {
            "Content-type": "application/json; charset=UTF-8"
        }
    });
}

async function post_api(api, body)
{
    const result = await fetch(`/api/${api}`, {
        method: "POST",
        body: JSON.stringify({body}),
        headers: 
        {
            "Content-type": "application/json; charset=UTF-8"
        }
    });

    if(result.status === 200)
    {
        alert("Der Vorgang wurde erfolgreich ausgeführt");
    }
    else if(result.status === 401)
    {
        alert("Fehler: Nicht angemeldet");
    }
    else if(result.status === 400)
    {
        const data = result.json();
        alert(`Es ist ein interner Serverfehler aufgetreten: ${data.errorMes}`);
    }
    else
    {
        alert("Ein unbekannter Fehler ist aufgetreten!");
    } 

    return result;
}

function displayCookieBox()
{
    let divCookies = document.getElementById("divCookies");
    if(!document.cookie.includes("cookiesAccepted="))
    {
        divCookies.style.visibility = "visible";
    }
}

function acceptCookies()
{
    let divCookies = document.getElementById("divCookies");
    divCookies.style.visibility="hidden";
    document.cookie = "cookiesAccepted=true; expires=Thu, 01 Jan 2100 00:00:00 UTC; path=/;";
}

function getUserBox()
{
    const divUserBox = document.createElement("div");
    divUserBox.classList = "box default_content";
    divUserBox.style.width = "200px";
    divUserBox.style.height = "100px";
    divUserBox.style.position = "fixed";
    divUserBox.style.top = "70px";
    divUserBox.style.right = "15px";
    divUserBox.style.gap = "0px"
    divUserBox.id = "divUserBox";
    divUserBox.addEventListener("mouseenter", () => {
        clearTimeout(userBoxTimeout);
    });
        divUserBox.addEventListener("mouseleave", () => {
        scheduleHideUserbox();
    });

    const btnAccMan = document.createElement("button");
    btnAccMan.textContent = "Account";
    btnAccMan.style.width = "200px";
    btnAccMan.style.height = "50px";    
    btnAccMan.style.fontSize = "20px";
    btnAccMan.addEventListener("click", () => {
        window.location.assign("/account.html");
    })
    divUserBox.appendChild(btnAccMan);

    const btnLogout = document.createElement("button");
    btnLogout.textContent = "Abmelden";
    btnLogout.style.width = "200px";
    btnLogout.style.height = "50px";
    btnLogout.style.fontSize = "20px";
    btnLogout.addEventListener("click", logOut);
    divUserBox.appendChild(btnLogout);

    return divUserBox;
}

function logOut()
{
    document.cookie = "session=; path=/; Max-Age=0";
    document.location.replace("/signin.html");
}

function scheduleHideUserbox()
{
    userBoxTimeout = setTimeout(() => {
        if (userBox) {
            userBox.remove();
            userBox = null;
        }
    }, 100);
}