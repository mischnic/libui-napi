const {AttributedString: AttributedStringC} = require('..');
const {FontAttribute} = require('./font-attribute');

class AttributedString {
	constructor(s) {
		this.handle = AttributedStringC.create(s);
	}

	forEach(cb) {
		AttributedStringC.forEach(this.handle, (attr, start, end) => {
			return Boolean(cb(this, new FontAttribute(attr), start, end));
		})
	}

	toString() {
		return AttributedStringC.toString(this.handle);
	}

	get length() {
		return AttributedStringC.toStringLen(this.handle);
	}

	appendUnattributed(s) {
		return AttributedStringC.appendUnattributed(this.handle, s);
	}

	insertUnattributed(s, pos) {
		return AttributedStringC.insertUnattributed(this.handle, s, pos);
	}

	deleteString(start, end) {
		return AttributedStringC.deleteString(this.handle, start, end);
	}

	setAttribute(attr, start, end) {
		// check attr
		return AttributedStringC.setAttribute(this.handle, attr.handle, start, end);
	}

	appendAttributed(s, ...attr) {
		return AttributedStringC.appendAttributed(this.handle, s,
												  attr.map(v => v.handle));
	}

	insertAttributed(s, pos, ...attr) {
		return AttributedStringC.insertAttributed(this.handle, s, pos,
												  attr.map(v => v.handle));
	}
}

module.exports = {AttributedString};
