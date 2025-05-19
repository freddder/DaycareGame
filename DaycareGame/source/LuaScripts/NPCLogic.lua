function get_next_line(dialog, input)
	if dialog.index ~= 0 and dialog[dialog.index].quit ~= nil then
		dialog.index = 0
		EndDialog()
		return
	end

	if dialog.index ~= 0 and dialog[dialog.index].choices ~= nil then -- has choices

		if input ~= 0 then
			dialog.index = dialog[dialog.index].choices[input].next -- go to specified choice
		else
			choices = {}
			for k,v in pairs(dialog[dialog.index].choices) do 
				table.insert(choices, v.text)
			end
			ShowDialogOptions(choices)
			return
		end			
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