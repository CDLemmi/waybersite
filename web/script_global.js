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