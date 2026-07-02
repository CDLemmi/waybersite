let tblPrivacy = document.getElementById("tblPrivacy");


onLoad();

async function onLoad()
{
    const response = await fetch("/content/legal.json");
    const data = await response.json();

    tblPrivacy.textContent=data.privacy;
}