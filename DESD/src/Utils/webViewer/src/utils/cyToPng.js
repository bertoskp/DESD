import { saveAs } from 'file-saver/FileSaver';

function b64toBlob(b64Data, contentType) {
    contentType = contentType || '';
    const sliceSize = 512;

    const byteCharacters = atob(b64Data);
    const byteArrays = [];

    for (let offset = 0; offset < byteCharacters.length; offset += sliceSize) {
        const slice = byteCharacters.slice(offset, offset + sliceSize);

        const byteNumbers = new Array(slice.length);
        for (var i = 0; i < slice.length; i++) {
            byteNumbers[i] = slice.charCodeAt(i);
        }

        const byteArray = new Uint8Array(byteNumbers);

        byteArrays.push(byteArray);
    }
    return new Blob(byteArrays, {type: contentType});
}

export function cyToPng(cy) {
    if (cy) {
        const b64key = 'base64,';
        const data = cy.png({full:true});
        const b64 = data.substring(data.indexOf(b64key) + b64key.length );
        const imgBlob = b64toBlob(b64, 'image/png');
        saveAs(imgBlob, 'graph.png');
    }
    else alert('First load a JSON.');
}