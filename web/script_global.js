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

onLoad();

function onLoad()
{
    displayCookieBox();
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