function get_next_line(dialog)
	if dialog.index ~= 0 and dialog[dialog.index].quit ~= nil then
		dialog.index = 0
		EndDialog()
		return
	end

	if dialog.index ~= 0 and dialog[dialog.index].choices ~= nil then
		dialog.index = dialog[dialog.index].choices[input].next
	else
		dialog.index = dialog.index + 1
	end

	if dialog[dialog.index] == nil then
		dialog.index = 1
		return
	end

	s = dialog[dialog.index].text
	ShowDialogLine(s)
	return
end