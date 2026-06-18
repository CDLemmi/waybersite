let tblAddress = document.getElementById("tblAddress");
let tblContact = document.getElementById("tblContact");

onload();

async function onload()
{
    const response = await fetch("/legal.json");
    const data = await response.json();

    tblAddress.innerHTML = `${data.impressum.name}<br>${data.impressum.address}<br>${data.impressum.city}`;
    tblContact.textContent = data.impressum.email;
}